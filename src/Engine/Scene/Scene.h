#ifndef __RENDER_SCENE_H__
#define __RENDER_SCENE_H__

#include "Engine/Model/Light.hpp"
#include "Engine/Model/Material.hpp"
#include "Engine/Model/Ray.hpp"
#include "Engine/Model/model.h"
#include "texture.hpp"
#include "tgaimage.h"
#include "tools/AABB.hpp"
#include <memory>
namespace render {
double DistributionGG(Vec3 N, Vec3 H, double roughness);
double GeometrySmith(Vec3 N, Vec3 V, Vec3 L, double roughness);
double fresnelSchlick(double cosTheta, double F0);
double fresnelSchlick_IBL(double cosTheta, double F0, double roughness);
// ray = a + tb
// class Model;
// class Ray {
// public:
//     Ray(Vec3 origin, Vec3 direction): origin_(origin), direction_(direction) {}
//     Vec3 at(double t) const {
//         return origin_ + direction_ * t;
//     }
//     Vec3 origin() const noexcept { return origin_; }
//     Vec3 direction() const noexcept { return direction_; }
// private:
//     Vec3 origin_;
//     Vec3 direction_;
// };

// class Material;
// class Intersection {
// public:
//     Vec3 p;  // 交点
//     Vec3 normal;

//     double t;
//     bool front; // ray is outside the sphere

//     // 让法线方向总是与光线方向相反
//     void setFaceNormal(const Ray& ray, const Vec3& outward_normal) {
//         // Sets the hit record normal vector.
//         // NOTE: the parameter `outward_normal` is assumed to have unit length.
//         front = (ray.direction() * outward_normal) < 0;  // 光线从外面射入
//         normal = front ? outward_normal : -1 * outward_normal;
//     }
// };
class Scene {
public:

    TGAColor color(Ray& ray);
    void add(std::shared_ptr<Model> model) { 
        models_.push_back(model); 
        // std::cout << " =================\n";
        // std::cout << model->bbBox().Min() << '\n';
        // std::cout << model->bbBox().Max() << '\n';
        bbox_ = Bounds(bbox_, model->bbBox());
    }
    void add(std::shared_ptr<Light> light) {
        lights_.push_back(light); 
    }; 

    Vec3 castRay(const Ray &ray, int depth) const;
    void sampleLight(Intersection& pos) const ;

    Bounds bbBox() const { return bbox_; }

    void setBackground(Vec3 background) { background_ = background; }
    void setDepth(int depth) { maxDepth = depth; }  
private:
    std::vector<std::shared_ptr<Model>> models_;
    std::vector<std::shared_ptr<Light>> lights_;
    int maxDepth = 50; // 光线弹射最大次数
    Bounds bbox_;
    Vec3 background_; // 背景颜色
};

}
#endif