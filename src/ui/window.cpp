#include "window.h"
#include "Engine/camera/camera.h"
#include "Engine/gl.h"
#include "GLFW/glfw3.h"
#include "tgaimage.h"
#include "ui/window.h"
#include <__msvc_chrono.hpp>
#include <chrono>
#include <gl/GL.h>


namespace render {
Camera camera;
double lastX = width / 2.f;
double lastY = height / 2.f;
bool first_mouse = true;

double deltaTime = 0.0;
std::chrono::time_point<std::chrono::steady_clock> lastFrame;
void framebuffersizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void updateTime() {
    static std::chrono::time_point<std::chrono::steady_clock> lastFrame = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> currentFrame = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - lastFrame).count() / 1000.0;
    lastFrame = currentFrame;
}

Window::Window(int width, int height, const char* title):  width_(width), height_(height), title_(title){}

void Window::init() {
    glfwInit();
    window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
    if(!window_) {
        glfwTerminate();
        LOG_ERROR("glfwCreateWindow Error");
    }
    glfwMakeContextCurrent(window_);
    // glViewport(0, 0, 800, 800);

    // 设置事件回调函数
    glfwSetFramebufferSizeCallback(window_, framebuffersizeCallback);
    glfwSetScrollCallback(window_, scrollCallback);
    glfwSetCursorPosCallback(window_, mouseCallback);
}


void Window::draw(TGAImage& image) {
    while(!glfwWindowShouldClose(window_)) {
        updateTime();
        processInput();
        glClear(GL_COLOR_BUFFER_BIT);
        // 渲染逻辑
        update(image);
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}
void Window::update(TGAImage& image) {
    glBegin(GL_POINTS);
    for(int i = 0; i < image.width(); i ++) {
        for(int j = 0; j < image.height(); ++j) {
            TGAColor color = image.get(i, j);
            glColor3f(1.0f * color[2] / 255.0f, 1.0 * color[1] / 255.0f, 1.0 * color[0] / 255.0f);
            glVertex3f( 1.0 * i / width_ * 2.0 - 1. , 1.0 * j / height_ * 2.0 - 1., 0.0);
            // std::cout << color[0] << " " << color[1] << " " << color[2] << '\n';
        }
    }
    glEnd();
}

Window::~Window() {
    glfwTerminate();
}


void framebuffersizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // std::cout << "x = " << xposIn << "y = " << yposIn << '\n';
    if(first_mouse) {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.processMouseMovement(xoffset, yoffset);
}
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll((double)yoffset);
}
void Window::processInput() {
    if(glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    } 
    if(glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if(glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWORD, deltaTime);
    }
    if(glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if(glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}
}