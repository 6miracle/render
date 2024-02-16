#ifndef __RENDER_RENDER_H__
#define __RENDER_RENDER_H__

#include "Engine/Model/model.h"
#include "pch.h"
#include <unordered_map>
#include <memory>
#include "Shader/shader.hpp"
namespace render {

class IRender {
public:
    virtual void render(const Scene& scene) = 0;
    virtual void clear() = 0;
};

// 光栅化渲染器
class Render:public IRender {
public:
    Render();
    ~Render() {
        image_.write_tga_file("image.tga");
        // zbuffer_.write_tga_file("depth.tga");
        for(int i = 0; i < model_.size(); ++i) {
            delete model_[i];
        }
    }
    void loadModel(const std::string& path);
    void loadModel(Node* node);
    void loadModel(Node& node, double radius);
    void loadShader(IShader* shader);
    virtual void render(const Scene& scene);
    // 光栅化
    void triangle(Node* nodes);      
    void cycle(Node& nodes, double radius);      
    void clear() { 
        image_.clear();
        std::fill(zbuffer_.begin(), zbuffer_.end(), 0);
     }
private:
    render::TGAImage image_;
    std::vector<double> zbuffer_;
    std::vector<Model*> model_;
    std::shared_ptr<IShader> shader_;
};

class TestRender: public Render {
public:

    void render(const Scene& scene) override;
private:

};
}

#endif