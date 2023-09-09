#include "model.h"
#include "tgaimage.h"
#include <algorithm>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>


namespace render {
Model::Model(const std::string& path) {
    std::ifstream is(path);
    ASSERT(is.is_open(), "文件打开失败");

    // std::ofstream os("ex1.txt");
    std::string str;
    while(std::getline(is, str)) {
        std::string s = str.substr(0, 2);
        if(s == "v ") {
            double x, y, z;
            std::istringstream(str.substr(2)) >> x >> y >> z;
            // os <<nodes.size() + 1 << " "<<  x  <<" " << y << " " << z <<"\n";
            nodes.push_back({x, y ,z});
            // os << nodes[nodes.size() - 1] << "\n";
        } else if(s == "vt") {
            double x, y;
            std::istringstream(str.substr(2)) >> x >> y;
            textures.push_back({x, y});
        } else if(s == "vn") {
            double x, y, z;
            std::istringstream(str.substr(2)) >> x >> y >> z;
            normals.push_back({x, y, z});
        } else if(s == "f ") {
            std::istringstream iss(str.substr(2));
            std::string tmp;
            // std::vector<Vec3> vec;
            Mat3 mat;
            while(std::getline(iss, tmp, ' ')) {
                double x, y, z;
                char trash;
                std::istringstream(tmp) >> x >> trash >> y >> trash >> z;
                mat << x - 1 << y - 1 << z - 1;
                face_node.push_back(x - 1);
                face_tex.push_back(y - 1);
                face_nor.push_back(z - 1);
            }
            faces.push_back(mat);
        } else {
            ASSERT(false, "INVALID");
        }
    }
    loadTexture(path, "_diffuse.tga", diffuseMap);
    loadTexture(path, "_nm.tga", normalMap);
    loadTexture(path, "_spec.tga", specularMap);
    loadTexture(path, "_nm_tangent.tga", normalTanMap);
}

std::string Model::ToString() const {
    std::stringstream ss;
    for(auto i : nodes) {
        ss << "v " << i << "\n";
    }
    for(auto i : textures) {
        ss << "vt " << i << "\n";
    }
    for(auto i : normals) {
        ss << "vn " << i << "\n";
    }
    for(auto i : faces) {
        ss << "f " << i <<"\n";
    }
    return ss.str();
}

void Model::loadTexture(const std::string& filename, const std::string suffix, TGAImage& image) {
    size_t dot = filename.find_last_of(".");
    if(dot == std::string::npos) return ;
    std::string texfile = filename.substr(0, dot) + suffix;
    std::cout <<"LoadTexture: " << texfile << ( image.read_tga_file(texfile) ? " ok" : " fail" ) << "\n";
    image.flip_vertically();
}

Vec3 Model::normal(Vec2 uv) {
    TGAColor color = normalMap.get(uv.x() * normalMap.width(), uv.y() * normalMap.height());
    Vec3 vec;
    for(int i = 0; i < 3; ++i) {
        vec[2 - i] = color[i] / 255.0 * 2.0 - 1.0;
    }
    return vec;
}
double Model::specular(Vec2 uv) {
    return specularMap.get(uv.x() * specularMap.width(), uv.y() * specularMap.height())[0] / 1.0;
}

Vec3 Model::normalTan(Vec2 uv) {
    TGAColor color = normalTanMap.get(uv.x() * normalTanMap.width(), uv.y() * normalTanMap.height());
    Vec3 vec;
    for(int i = 0; i < 3; ++i) {
        vec[2 - i] = color[i] / 255.0 * 2.0 - 1.0;
    }
    return vec;
}
}