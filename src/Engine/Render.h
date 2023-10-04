#ifndef __RENDER_RENDER_H__
#define __RENDER_RENDER_H__

#include "Engine/Model/model.h"
#include "pch.h"
#include <unordered_map>
#include <memory>
#include "Shader/shader.hpp"
namespace render {

class Render {
public:
    Render();
    ~Render() {
        image_.write_tga_file("image.tga");
        // zbuffer_.write_tga_file("depth.tga");
    }
    void loadModel(const std::string& path);
    void loadShader(IShader* shader);
    // 光栅化
    void triangle(Node* nodes);            
    void render();
    // Model* getModel()  { return model_;}
    void clear() { 
        image_.clear();
        std::fill(zbuffer_.begin(), zbuffer_.end(), 0);
     }
    void write() {
        image_.write_tga_file("image.tga");
        // zbuffer_.write_tga_file("zbuffer.tga");
    }
private:
    render::TGAImage image_;
    std::vector<double> zbuffer_;
    std::vector<Model*> model_;
    std::shared_ptr<IShader> shader_;
};
}

#endif