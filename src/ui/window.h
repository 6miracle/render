#ifndef __RENDER_WINDOW_H__
#define __RENDER_WINDOW_H__

#include "Engine/Render.h"
#include "Engine/Scene/Scene.h"
#include "Engine/camera/camera.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "pch.h"
#include "tgaimage.h"
#include "util.hpp"

namespace render {
#define ASSERT1(x) if(!(x)) exit(0);
#define GL_CHECK(x) GLClearError(); \
        x; \
        ASSERT1(GLGetError(#x, __FILE__, __LINE__)); 
class IWindow {
public:
    virtual void init() = 0;
    virtual void loadRender(IRender* render) = 0;
    virtual void loadScene(Scene* scene) = 0 ;
    virtual void draw() = 0;
};


class Window: public IWindow {
public:
    Window(int width = 800, int height = 800, const char* title = "window");
    ~Window();
    void init();
    void loadRender(IRender* render);
    void loadScene(Scene* scene);
    void draw();
    void processInput();
private:
    GLFWwindow* window_;
    int width_;
    int height_;
    std::string title_;
    IRender* render_;
    Scene* scene_;
};


class Editor: public IWindow {
public:
    Editor(int width_ = width, int height_ = height);
    ~Editor() = default;
    void init() override;
    void loadRender(IRender* render) override;
    void loadScene(Scene* scene) override;
    void draw() override;
    void setName(const std::string& filename) { filename_ = filename + ".tga"; } 
private:
    int width_;
    int height_;

    IRender* render_;
    Scene* scene_;
    TGAImage image_;
    std::string filename_;
};
}

#endif