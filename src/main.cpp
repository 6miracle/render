#include "Engine/Model/model.h"
#include "Engine/Render.h"
#include "Engine/camera/camera.h"
#include "Shader/shader.hpp"
#include "maths/Matrix.hpp"
#include "pch.h"
#include "tools/load.hpp"
#include "ui/window.h"
#include "nlohmann/json.hpp"
#include <fstream>
const int width  = 800;
const int height = 800;
const int depth = 255;

using render::Vec3;
using render::Vec2;
using render::Vec4;
using render::Mat3;
using  render::Mat4;
Vec3 light_dir{0 , 0, 1};

void jsonTest() {
    std::ifstream ifs("test.json");
    nlohmann::json data = nlohmann::json::parse(ifs);
    std::cout << data << '\n';
}

int main() {
    render::Render render;
    render.loadModel(load());
    render.loadShader(new render::GouraudShader());

    render::Window window;
    window.init();
    window.loadRender(&render);
    window.draw();
    // render.render();

    // render::Model model(load());
    // std::cout << model.model() << '\n';
    // render.loadJson(load());

}