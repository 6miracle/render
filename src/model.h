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
class Model {
public:
    Model(const std::string& path);
    // bool Load(const std::string& path);

    Vec3 node(int i) { return nodes[i]; }
    Vec3 node(int face, int i) { return nodes[face_node[face * 3 + i]]; }
    Vec2 uv(int face, int i) { return textures[face_tex[face * 3 + i]]; }
    const TGAImage& diffuse() { return diffuseMap; }
    const TGAImage& specular() { return specularMap; }
    std::string ToString() const;

    size_t nfaces() const { return faces.size(); }
    Mat3 face(size_t i) const { return faces[i]; }

    void loadTexture(const std::string& filename, const std::string suffix, TGAImage& image);
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
};
}


#endif