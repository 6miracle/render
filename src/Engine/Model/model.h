#ifndef __RENDER_MODEL_H__
#define __RENDER_MODEL_H__

#include "Engine/Model/Intersection.hpp"
#include "Engine/Model/Material.hpp"
// #include "Engine/Scene/Scene.h"
#include "pch.h"
#include "maths/maths.hpp"
#include "maths/Matrix.hpp"
#include "texture.hpp"
#include "tgaimage.h"
#include "tools/AABB.hpp"
#include "util.hpp"
#include <array>
#include <cmath>
#include <corecrt.h>
#include <fstream>
#include <memory>
#include <numbers>
#include <ostream>
// #include <xtimec.h>
namespace render {

// 面
struct Node {
    Vec4 coords; // 世界坐标 -- 裁剪坐标
    Vec4 screen_coords;  // 屏幕坐标
    Vec2 uv;        // 纹理坐标
    Vec3 normal;    // 法向量
    Vec3 normalTan; // 法线
    TGAColor diffuse; // 环境光
    double specular; // 高光
};
class Model {
public:
    enum class Type {
        Base, Triangle, Circle, Ball, QUAD, Object, BVH
    };
    virtual void Node(struct Node& tmp,int face, int i){};
    virtual Mat4 model() { return Mat4{}; }
    virtual size_t nfaces() const {return 0;}
    virtual Type type() const {  return Type::Base; }
    virtual bool hittable() const { return true;} // 是否可与光线相交
    virtual bool hit(const Ray& r, double ray_tmin, double ray_tmax, Intersection& rec) const{ return false; }
    virtual Bounds bbBox() const { return Bounds{}; }
};  
class TriModel: public Model {
public:
    TriModel(struct Node* node);
    void Node(struct Node& tmp,int face, int i) override;
    Mat4 model() override;
    size_t nfaces() const override{return 3;}
    Type type() const override {  return Type::Triangle; }
private:
    std::array<struct Node, 3> nodes_;
    Mat4 modelMat_;
};

class CircleModel: public Model {
public:
    CircleModel(struct Node& node, double radius);
    void Node(struct Node& tmp,int face, int i) override;
    Mat4 model() override;
    size_t nfaces() const override { return radius_ * 100;}
    Type type() const override {  return Type::Circle; }
    double getRadius() { return radius_; }
private:
    struct Node center_;
    double radius_;
};

class BallModel: public Model {
public:
    BallModel(struct Node& node, double radius);
    void Node(struct Node& tmp,int face, int i) override;
    Mat4 model() override;
    size_t nfaces() const override { return total_;}
    Type type() const override {  return Type::Ball; }
    double getRadius() { return radius_; }
    TGAColor cubeMap(int face, Vec3 dir);
    TGAColor prefilterMap(Vec3 ref, int level);
    TGAColor irradianceMap(Vec3 normal); // 利用法线进行采样
    TGAColor brdfLUT(Vec2 vec);
    void loadCubeMap(const std::string& path);
private:
    TGAImage cubeMaps_[6];
    TGAImage irradianceMap_; // 漫反射IBL贴图
    TGAImage prefilterMap_; // 镜面反射
    TGAImage brdfLUT_; // 
    struct Node center_;
    double radius_;
    double total_;
};

// 矩形
class Quad: public Model {
public:
    Quad(const Vec3& q, const Vec3&u ,const Vec3& v, std::shared_ptr<Material> m)
        :q_(q), u_(u), v_(v), mat_(m) {
            auto n = u.cross(v);
            normal_ = n.normalized();
            D_ = normal_ * q;
            w_ = n / (n * n);
            setBbox();
        }

        virtual void setBbox() {
            bbox_ = Bounds(q_, q_ + u_ + v_).pad();
        }
        Bounds bbBox() const override {
            return bbox_;
        }
        bool hit(const Ray& r, double ray_tmin, double ray_tmax, Intersection& rec) const override {
            double denom = normal_ * r.direction();

            // 如果光线与平面平行，显然不命中
            if(std::fabs(denom) < 1e-8) {
                // std::cout << "parallel\n";
                return false;
            }

            // 求交点
            double t = (D_ - normal_ * r.origin()) / denom;
            if(t < ray_tmin || t > ray_tmax) {
                // std::cout << "max\n";
                return false;
            }

            // Determine the hit point lies within the planar shape using its plane coordinates.
            auto intersection = r.at(t);
            Vec3 planar_hitpt_vector = intersection - q_;
            auto alpha = w_ * (planar_hitpt_vector.cross(v_));
            auto beta = w_ * (u_.cross(planar_hitpt_vector));

            if(!is_interior(alpha, beta, rec)) {
                // std::cout << "inter\n";
                return false;
            }

            // Ray hits the 2D shape; set the rest of the hit record and return true.
            rec.t = t;
            rec.p = intersection;
            rec.mat = mat_;
            rec.setFaceNormal(r, normal_);
            return true; 
        }

        virtual bool is_interior(double a, double b, Intersection& rec) const {
            if((a < 0) || (a > 1) || (b < 0) || (b > 1)) {
                return false;
            }
            rec.uv = Vec2{a, b};
            return true;
        }

private:
    Vec3 q_;    // 左下角的点
    Vec3 u_, v_; // 俩个方向向量
    Bounds bbox_;
    std::shared_ptr<Material> mat_; 
    Vec3 normal_;  // 平面法向量
    double D_;  // 平面方程Ax + By +Cx = D 的常数D
    Vec3 w_; // 用于平面上定位点
};

class Sphere: public Model {
public:

    Sphere(Vec3 center, double radius): center_(center), radius_(radius), is_moving_(false) {}
    Sphere(Vec3 center, double radius, std::shared_ptr<Material> mat): 
        center_(center), radius_(radius), mat_(mat), is_moving_(false){
            Vec3 rVec = Vec3{radius, radius, radius};
            bbox = Bounds(center - rVec, center + rVec);
        }
    Sphere(Vec3 begin, Vec3 end, double radius, std::shared_ptr<Material> mat): 
        center_(begin), radius_(radius), mat_(mat), is_moving_(true){
            center_vec_ = end - begin;
            Vec3 rVec = Vec3{radius, radius, radius};
            Bounds bbox1 = Bounds(begin - rVec, begin + rVec);
            Bounds bbox2 = Bounds(end - rVec, end + rVec);
            bbox = Bounds(bbox1, bbox2);
        }
    bool hit(const Ray& ray, double ray_tmin, double ray_tmax, Intersection& rec) const override { 
        Vec3 scenter = is_moving_ ? center(ray.time()) : center_;
        Vec3 ac = ray.origin() - scenter;
        double half_b = ac * ray.direction();
        double a = ray.direction() * ray.direction();

        double c = ac * ac - radius_ * radius_;
        double dis = half_b * half_b -  a * c ;
        if(dis < 0) { return false; }
        double tmp = std::sqrt(dis);
        double root = (-half_b - tmp) / a;
        if(root < ray_tmin || root > ray_tmax) {
            root = (-half_b + tmp) / a;
            if(root < ray_tmin || root > ray_tmax) {
                return false;
            }
        }

        rec.t = root;
        rec.p = ray.at(root);
        rec.mat = mat_;
        Vec3 outward_normal = (rec.p - center_) / radius_;
        rec.uv = uv(outward_normal);
        rec.setFaceNormal(ray, outward_normal);
        return true;
    }

    Bounds bbBox() const override { return bbox; }

    static Vec2 uv(const Vec3& p) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
        // 由当前坐标得出uv坐标
        double theta = std::acos(-p.y());
        double phi = std::atan2(-p.z(), p.x()) + std::numbers::pi;

        return Vec2{phi / (2 * std::numbers::pi), theta / std::numbers::pi};

    }
private:    
    Vec3 center_;
    double radius_;
    std::shared_ptr<Material> mat_;
    bool is_moving_;
    Vec3 center_vec_;
    Bounds bbox;
    Vec3 center(double time) const {
        return center_ + time * center_vec_;
    }
};

// 达到旋转/平移的效果
class Translate: public Model {
public:
    Translate(std::shared_ptr<Model> object, const Vec3& offset)
        : object_(object), offset_(offset)  {
            bbox_ = object_->bbBox() + offset_;
        }

    bool hit(const Ray& r, double ray_tmin, double ray_tmax, Intersection& rec) const override { 
        // Move the ray backwards by the offset
        Ray ray(r.origin() - offset_, r.direction(), r.time());

        // // Determine where (if any) an intersection occurs along the offset ray
        if(!object_->hit(ray, ray_tmin, ray_tmax, rec)) {
            return false;
        }
        // Move the intersection point forwards by the offset
        rec.p += offset_;
        return true; 
    }

    Bounds bbBox() const override { return bbox_; }
private:
    std::shared_ptr<Model> object_;
    Vec3 offset_;
    Bounds bbox_;
};

// 沿y轴旋转
class YRotate: public Model {
public:
    YRotate(std::shared_ptr<Model> obj, double angle): object_(obj)  {
        double radius = degree_to_radians(angle);
        sin_ = std::sin(radius);
        cos_ = std::cos(radius);

        auto bbox = obj->bbBox();
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        Vec3 pMin = Vec3{maxNum, maxNum, maxNum};
        Vec3 pMax = Vec3{minNum, minNum, minNum};
        for(int i = 0; i < 2; ++i) {
            for(int j = 0; j < 2; ++j) {
                for(int k = 0; k < 2; ++k) {
                    // double x = (1 - i) * bbox.Min().x() + i * bbox.Max().x();
                    // double y = (1 - j) * bbox.Min().y() + j * bbox.Max().y();
                    // double z = (1 - k) * bbox.Min().z() + k * bbox.Max().z();
                    double x = i * bbox.Min().x() + (1 - i) * bbox.Max().x();
                    double y = j * bbox.Min().y() + (1 - j) * bbox.Max().y();
                    double z = k * bbox.Min().z() + (1 - k) * bbox.Max().z();

                    double xx = cos_ * x + sin_ * z;
                    double zz = -sin_ * x + cos_ * z;
                    pMin = Vec3{ std::min(pMin.x(), xx), std::min(pMin.y(), y), std::min(pMin.z(), zz) };
                    pMax = Vec3{ std::max(pMax.x(), xx), std::max(pMax.y(), y), std::max(pMax.z(), zz) };
                }
            }
        }
        box_ = Bounds{pMin, pMax};
    }

    bool hit(const Ray& r, double ray_tmin, double ray_tmax, Intersection& rec) const override { 
        // 将光线从世界空间转到物体空间
        Vec3 origin = r.origin();
        Vec3 direction = r.direction();

        origin[0] = cos_ * r.origin()[0] - sin_ * r.origin()[2];
        origin[2] = sin_ * r.origin()[0] + cos_ * r.origin()[2];

        direction[0] = cos_ * r.direction()[0] - sin_ * r.direction()[2];
        direction[2] = sin_ * r.direction()[0] + cos_ * r.direction()[2];

        Ray ray(origin, direction, r.time());
        if(!object_->hit(ray, ray_tmin, ray_tmax, rec)) {
            return false;
        }
        auto p = rec.p;
        p[0] = cos_ * rec.p[0] + sin_ * rec.p[2];
        p[2] = -sin_ * rec.p[0] + cos_ * rec.p[2];

        auto normal = rec.normal;
        normal[0] = cos_ * rec.normal[0] + sin_ * rec.normal[2];
        normal[2] = -sin_ * rec.normal[0] + cos_ * rec.normal[2];

        rec.p = p;
        rec.normal = normal;
        return true;
    }

    Bounds bbBox() const override { return box_; } 
private:    
    std::shared_ptr<Model> object_;
    double sin_;
    double cos_;
    Bounds box_;
};


// 恒定密度介质, 气体
class  ConstantMedium: public Model {
public:
    ConstantMedium(std::shared_ptr<Model> boundary, double density, std::shared_ptr<Texture> mat)
        : boundary_(boundary), neg_inv_density_(-1 / density), mat_(std::make_shared<Isotropic>(mat)) {}
    ConstantMedium(std::shared_ptr<Model> boundary, double density, Vec3 mat)
        : boundary_(boundary), neg_inv_density_(-1 / density), mat_(std::make_shared<Isotropic>(mat)) {}
    bool hit(const Ray& r, double ray_tmin, double ray_tmax, Intersection& rec) const override { 
        const bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 0.0001;

        Intersection rec1, rec2;
        double Max = std::numeric_limits<double>::max();
        double Min = std::numeric_limits<double>::lowest();
        if(!boundary_->hit(r, Min, Max, rec1)) {
                return false;
        }
        if(!boundary_->hit(r, rec1.t + 0.0001, Max, rec2)) {
            return false;
        }
        if (debugging) std::clog << "\nray_tmin=" << rec1.t << ", ray_tmax=" << rec2.t << '\n';
        if(rec1.t < ray_tmin) { rec1.t = ray_tmin; }
        if(rec2.t > ray_tmax) { rec2.t = ray_tmax; }

        if(rec1.t >= rec2.t) {
            return false;
        }

        if(rec1.t < 0) {
            rec1.t = 0;
        }

        auto ray_length = r.direction().norm();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density_ * std::log(random_double());

        if(hit_distance > distance_inside_boundary) {
            return false;
        }
        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging) {
            std::clog << "hit_distance = " <<  hit_distance << '\n'
                      << "rec.t = " <<  rec.t << '\n'
                      << "rec.p = " <<  rec.p << '\n';
        }

        rec.normal = Vec3{1,0,0};  // arbitrary
        rec.front = true;     // also arbitrary
        rec.mat = mat_;

        return true;
    }
private:    
    std::shared_ptr<Model> boundary_;
    double neg_inv_density_; 
    std::shared_ptr<Material> mat_;
};

class ObjModel:public Model{
public:
    ObjModel(const std::string& path);
    void load(const std::string& path);
    void Node(struct Node& tmp,int face, int i) override{
        tmp.coords = local2homo(node(face, i));
        if(!normals.empty()) { tmp.normal = normal(face, i); }
        if(!textures.empty())  { tmp.uv = uv(face, i); }
        if(!normalTanMap.empty()) { tmp.normalTan = normalTan(tmp.uv); }
        if(!diffuseMap.empty()) {tmp.diffuse = diffuse(tmp.uv); }
        if(!specularMap.empty()) { tmp.specular = specular(tmp.uv); }
    }
    Vec3 node(int i) { return nodes[i]; }
    Vec3 node(int face, int i) { return nodes[face_node[face * 3 + i]]; }
    Vec3 normal(int face, int i) { return normals[face_nor[face * 3 + i]]; }
    Vec3 normal(Vec2 uv);

    Vec3 normalTan(Vec2 uv);

    Vec2 uv(int face, int i) { return textures[face_tex[face * 3 + i]]; }
    const TGAImage& diffuse() { return diffuseMap; }
    TGAColor diffuse(Vec2 uv) { return diffuseMap.get(uv.x() * diffuseMap.width(), uv.y() * diffuseMap.height());}
    const TGAImage& specular() { return specularMap; }
    double specular(Vec2 uv);
    std::string ToString() const;

    size_t nfaces() const override{ return faces.size(); }
    Mat3 face(size_t i) const { return faces[i]; }

    // void loadTexture(const std::string& filename, const std::string suffix, TGAImage& image);
    void loadObj(const std::string& path);
    Mat4 model() override{ return modelMatrix_; }
    void test(std::ofstream& os) {
       for(size_t i = 0; i < face_tex.size(); i +=3) {
        os << face_tex[i] << " " << face_tex[i + 1] << " " << face_tex[i + 2] << "\n";
       }
       for(size_t i = 0; i < textures.size(); ++i) {
        os << textures[i] << "\n";
       }
    }
    Type type() const override {  return Type::Object; }
private:    
    std::vector<Vec3> nodes;
    std::vector<Vec2> textures;
    std::vector<Vec3> normals;

    std::vector<Mat3> faces;
    std::vector<int> face_node;
    std::vector<int> face_tex;
    std::vector<int> face_nor;
    // 纹理贴图
    // 漫反射贴图
    TGAImage diffuseMap{}; 
    // 法线贴图
    TGAImage normalMap{};
    // 高光贴图
    TGAImage specularMap{};

    TGAImage normalTanMap{};

    Mat4 modelMatrix_;
};


class Objects:public Model {
public:
    Objects() =default;
    Objects(std::shared_ptr<Model> model) { add(model); }

    void clear() { models_.clear(); }
    void add(std::shared_ptr<Model> model) {
        models_.push_back(model); 
        bbox_ = Bounds(bbox_, model->bbBox());
    }   

    Bounds bbBox() const override { return bbox_; }
    bool hit(const Ray& ray, double ray_tmin, double ray_tmax, Intersection& rec) const override { 
        bool flag = false;
        Intersection tmp;
        double tFar = ray_tmax;
        for(const auto& model : models_) {
           if(model->hit(ray, ray_tmin, tFar, tmp)) {
            flag = true;
            tFar = tmp.t; 
            rec = tmp;
           }
        }
        return flag;
    }
    std::vector<std::shared_ptr<Model>>& objects() { return models_; }
    int size() const { return models_.size(); }
private:
    std::vector<std::shared_ptr<Model>> models_;
    Bounds bbox_;
};


inline std::shared_ptr<Objects> box(const Vec3& a, const Vec3& b, std::shared_ptr<Material> mat) {
    auto sides = std::make_shared<Objects>();
    Vec3 pMin{std::min(a.x(), b.x()), std::min(a.y(), b.y()), std::min(a.z(), b.z())};
    Vec3 pMax{std::max(a.x(), b.x()), std::max(a.y(), b.y()), std::max(a.z(), b.z())};

    Vec3 dx{pMax.x() - pMin.x(), 0., 0.};
    Vec3 dy{0., pMax.y() - pMin.y(), 0.};
    Vec3 dz{0., 0., pMax.z() - pMin.z()};

    sides->add(std::make_shared<Quad>(Vec3{pMin.x(), pMin.y(), pMax.z()}, dx, dy, mat));  // front
    sides->add(std::make_shared<Quad>(Vec3{pMax.x(), pMin.y(), pMax.z()}, -1 * dz, dy, mat));  // // right
    sides->add(std::make_shared<Quad>(Vec3{pMax.x(), pMin.y(), pMin.z()}, -1 * dx, dy, mat));    // back
    sides->add(std::make_shared<Quad>(Vec3{pMin.x(), pMin.y(), pMin.z()}, dz, dy, mat)); // left
    sides->add(std::make_shared<Quad>(Vec3{pMin.x(), pMax.y(), pMax.z()}, dx, -1 * dz, mat)); // top
    sides->add(std::make_shared<Quad>(Vec3{pMin.x(), pMin.y(), pMin.z()},  dx, dz, mat));  // bottom

    return sides;
}
}


#endif