#include "maths/Matrix.hpp"
#include "maths/maths.hpp"
#include "GL/gl.h"
#include "model.h"
#include "tgaimage.h"
#include "pch.h"
#include "util.h"
#include <chrono>
#include <fstream>
#include <limits>
#include <ostream>
#include <stdint.h>
#include <stdlib.h>

using render::Vec3;
using render::Vec2;
using render::Vec4;
using render::Mat3;
using  render::Mat4;

render::Model* model;
const render::TGAColor white = render::TGAColor {.bgra = {255, 255, 255, 255}};
const render::TGAColor red   = render::TGAColor{255, 0,   0};
const render::TGAColor green = render::TGAColor{0,   255,  0,  255, 4};
const render::TGAColor blue = render::TGAColor{ .bgra = {0, 0, 255, 255} };

const int width  = 800;
const int height = 800;
const int depth = 255;

Vec3 light_dir{1, 1, 1};
Vec3 eye{0, -1, 3};
Vec3 center{0, 0, 0};
Vec3 up{0, 1, 0};

  std::ofstream os("ex1.txt");
// 每个顶点计算颜色然后插值
class GouraudShader: public render::IShader {
public:
    Vec4 vertex(int face, int nthvert) {
      Vec4 node = render::local2homo(model->node(face, nthvert));
      uvs[nthvert] = model->uv(face , nthvert);
      node = render::viewportmat * render::projectionmat * render::viewmat * render::modelmat* node;
      intensity_[nthvert] = std::max(0., model->normal(face , nthvert) * light_dir);
      return node;
    }
    bool fragment(Vec3 vec, render::TGAColor& color) {
      double intensity = vec * intensity_;
      intensity = intensity > 1.0 ? 1.0  : (intensity < 0 ? 0 : intensity);
      Vec2 uv = uvs[0] * vec[0] + uvs[1] * vec[1] + uvs[2] * vec[2];
      color = model->diffuse().get(uv.x() * model->diffuse().width(), uv.y() * model->diffuse().height());
      return false;
    } 
private:
      Vec3 intensity_;
      Vec2 uvs[3];
};



int main() {
  // test();

  model = new render::Model("../obj/african_head.obj");

  render::modelmatrix(0);
  render::viewMatrix(eye, center, up);
  render::projectionMatrix(-1.f/(eye-center).norm());
  render::viewPortMatrix(width * 3 / 4, height * 3 / 4);

  light_dir = light_dir.normalized();
  os << "model\n" << render::modelmat << "\n";
  os << "view\n" << render::viewmat << "\n";
  os << "projection\n" << render::projectionmat << "\n";
  os <<"viewport\n" << render::viewportmat << "\n";

  render::TGAImage image(width, height, 3);
  render::TGAImage zbuffer(width, height, 1);
  GouraudShader shader;

  // model.test(os);
  for (int i = 0; i < model->nfaces(); i++) { 
    render::Vec4 screen_coords[3]; 
    for (int j = 0; j < 3; j++) { 
      screen_coords[j] = shader.vertex(i, j);
    } 
    triangle(screen_coords, shader, image, zbuffer); 
    // os << model->node(i, 0) << "\n" << model->node(i, 1) << "\n" << model->node(i, 2) << "\n";
  }

  // test(image);
  image.write_tga_file("example.tga");
  zbuffer.write_tga_file("example.depth.tga");
  delete model;
  return 0;
}