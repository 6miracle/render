#ifndef __RENDER_RENDER_H__
#define __RENDER_RENDER_H__

#include "Engine/Model/model.h"
#include "Engine/gl.h"
#include "pch.h"
namespace render {

class Render {
public:
    Render(const std::string& path);
    void loadShader(IShader* shader);
    void render();

private:
    Model model_;
    std::shared_ptr<IShader> shader_;
};
}

#endif