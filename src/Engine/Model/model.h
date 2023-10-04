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
// typedef std::array<float, 3> Vec3;
// typedef std::array<float, 2> Vec2;
// typedef std::array<std::array<int, 3>, 3> Mat3x3;

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
    Model(const std::string& path);
    // bool Load(const std::string& path);

    void Node(struct Node& tmp,int face, int i) {
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

    size_t nfaces() const { return faces.size(); }
    Mat3 face(size_t i) const { return faces[i]; }

    void loadTexture(const std::string& filename, const std::string suffix, TGAImage& image);
    void loadObj(const std::string& path);
    Mat4 model() { return modelMatrix_; }
    void test(std::ofstream& os) {
       for(size_t i = 0; i < face_tex.size(); i +=3) {
        os << face_tex[i] << " " << face_tex[i + 1] << " " << face_tex[i + 2] << "\n";
       }
       for(size_t i = 0; i < textures.size(); ++i) {
        os << textures[i] << "\n";
       }
    }

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