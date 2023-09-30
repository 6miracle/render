#ifndef __RENDER_RENDER_H__
#define __RENDER_RENDER_H__

#include "Engine/Model/model.h"
#include "pch.h"
#include <unordered_map>
#include <memory>
namespace render {

class IShader {
public:
    virtual ~IShader() = default;
    virtual Vec4 vertex(int face, int nthvert) = 0;
    virtual bool fragment(Vec3 vec, TGAColor& color) = 0;
    void setUniform(const std::string& name, const Mat4& mat);
    void setModel(Model* model) { model_ = model;}
protected:
    std::unordered_map<std::string, Mat4> map_;
    Model* model_;

};

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
    void triangle(Vec4 vecs[3]);            
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