#ifndef __RENDER_WINDOW_H__
#define __RENDER_WINDOW_H__

#include "Engine/Render.h"
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

class Window {
public:
    Window(int width = 800, int height = 800, const char* title = "window");
    ~Window();
    void init();
    void loadRender(Render* render);
    void draw();
    void processInput();
private:
    GLFWwindow* window_;
    int width_;
    int height_;
    std::string title_;
    Render* render_;
};
}

#endif