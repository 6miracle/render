#include "window.h"
#include "GLFW/glfw3.h"
#include "ui/window.h"
#include <gl/gl.h>

namespace render {
void Window::draw() {
    while(!glfwWindowShouldClose(window)) {
        update();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
void Window::update() {
    glBegin(GL_POINT);

    for(int i = 0; i < 100; ++i) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(i, i, 0.0);
    }
    glEnd();
}

Window::~Window() {
    glfwTerminate();
}

}