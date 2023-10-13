#ifndef __RENDER_MODEL_H__
#define __RENDER_MODEL_H__

#include "pch.h"
#include "maths/maths.hpp"
#include "maths/Matrix.hpp"
#include "tgaimage.h"
#include <array>
#include <fstream>
#include <ostream>
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
        Base, Triangle, Circle, Ball, Object
    };
    virtual void Node(struct Node& tmp,int face, int i) = 0;
    virtual Mat4 model() = 0;
    virtual size_t nfaces() const = 0;
    virtual Type type() const {  return Type::Base; }
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
}


#endif