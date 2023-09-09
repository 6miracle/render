#ifndef __RENDER_WINDOW_H__
#define __RENDER_WINDOW_H__

#include "GLFW/glfw3.h"
#include "pch.h"
#include "util.h"

namespace render {
    class Window {
    public:
        Window(int width = 800, int height = 800, const char* title = "window") {
            glfwInit();
            window = glfwCreateWindow(width, height, title, nullptr, nullptr);
            if(!window) {
                glfwTerminate();
                LOG_ERROR("glfwCreateWindow Error");
            }
            glfwMakeContextCurrent(window);
        }
        ~Window();

        void draw();
        void update();
    private:
        GLFWwindow* window;
    };
}

#endif