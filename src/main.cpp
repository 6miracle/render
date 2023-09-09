#include "Shader/shader.hpp"
#include "maths/Matrix.hpp"
#include "maths/maths.hpp"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "GL/gl.h"
#include "model.h"
#include "tgaimage.h"
#include "pch.h"
#include "ui/window.h"
#include "util.h"
#include <chrono>
#include <cmath>
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

Vec3 light_dir{0 , 0, 1};
Vec3 eye{0, 1, 3};
Vec3 center{0, 0, 0};
Vec3 up{0, 1, 0};

std::ofstream os("ex1.txt");
// // 每个顶点计算颜色然后插值
// class GouraudShader: public render::IShader {
// public:
//     Vec4 vertex(int face, int nthvert) {
//       Vec4 node = render::local2homo(model->node(face, nthvert));
//       uvs[nthvert] = model->uv(face , nthvert);
//       return render::viewportmat * render::projectionmat * render::viewmat * render::modelmat* node;
//     }
//     bool fragment(Vec3 vec, render::TGAColor& color) {
//       Vec2 uv = uvs[0] * vec[0] + uvs[1] * vec[1] + uvs[2] * vec[2];
//       Vec3 n = render::homo2local(uniform_MIT * render::local2homo(model->normal(uv))).normalized();
//       Vec3 l = render::homo2local(uniform_M * render::local2homo(light_dir)).normalized();
//       // 反射光线
//       Vec3 r = (2 * n * (n* l) - l).normalized();
//       double diff = std::max(0.0, n * l);
//       double specular = pow(std::max(r.z(), 0.), model->specular(uv));
//       // double intensity = std::max(0.0, n * l);
//       render::TGAColor cl = model->diffuse(uv);
//       color = cl;
//       for(int i = 0; i < 3; ++i) color[i] = std::min(255.0, 5.0 + cl[i] * (diff + 6 * specular));
//       return false;
//     } 

//     void init() {
//       uniform_M =  render::viewmat * render::modelmat;
//       uniform_MIT = (render::projectionmat * uniform_M).invert().transpose();
//       // os << uniform_M <<"\n";
//       // os <<"----------\n";
//       // os << uniform_MIT <<"\n";
//       // os <<"========================\n";
//     }
// private:
//       Vec3 intensity_;
//       Vec2 uvs[3];
//       Mat4 uniform_M;
//       Mat4 uniform_MIT;
// };


// class PhongShader:public render::IShader {
// public:
//   Vec4 vertex(int face, int nthvert) {
//     triangle[nthvert] = model->node(face, nthvert);
//     Vec4 node = render::local2homo(model->node(face, nthvert));
//     uvs[nthvert] = model->uv(face, nthvert);
//     // normals[nthvert] = model->normal(face, nthvert);
//     return render::viewportmat * render::projectionmat * render::viewmat * render::modelmat* node;
//   } 
//   bool fragment(Vec3 vec, render::TGAColor& color) {
//     Vec2 uv = uvs[0] * vec[0] + uvs[1] * vec[1] + uvs[2] * vec[2];
//     // Vec3 normal = (normals[0] * vec[0] + normals[1] * vec[1] + normals[2] * vec[2]).normalized();

//     Vec3 normalTan = model->normalTan(uv).normalized();
//     // Vec3 normalTan = Vec3{c[2] / 255.0 * 2.0 - 1 , c[1] / 255.0 * 2.0 - 1, c[0] / 255.0 * 2.0 - 1}.normalized();
//     double u0 = uvs[1].x() - uvs[0].x();
//     double u1 = uvs[2].x() - uvs[0].x();
//     double v0 = uvs[1].y() - uvs[0].y();
//     double v1 = uvs[2].y() - uvs[0].y();
//     Vec3 e0 = { (uvs[1] - uvs[0]).x(), (uvs[1] - uvs[0]).y(), 0.};
//     Vec3 e1 = { (uvs[2] - uvs[0]).x(), (uvs[2] - uvs[0]).y(), 0.};
//     render::Mat<2, 2> mat1{v1, -v0, -u1, u0};
//     render::Mat<2, 3> mat2{e0.x(), e0.y(), e0.z(), e1.x(), e1.y(), e1.z()};
//     render::Mat<2, 3> mat = mat1 * mat2 / (u0 * v1 - v0 * u1);
//     Vec3 t{mat[0][0], mat[0][1], mat[0][2]};
//     Vec3 b{mat[1][0], mat[1][1], mat[1][2]};
//     t.normalized();
//     b.normalized();
//     Vec3 n = t.cross(b).normalized();

//     Mat4 matrix;
//     matrix << t.x(), b.x(), n.x(), 0,
//               t.y(), b.y(), n.y(), 0,
//               t.z(), b.z(), n.z(), 0,
//               0, 0, 0, 1;
//     Vec4 nor = (matrix * Vec4{normalTan.x(), normalTan.y(), normalTan.z(), 0.}).normalized();
//     nor = (render::viewmat * render::modelmat).invert().transpose() * nor;
//     Vec3 normal{nor.x(), nor.y(), nor.z()};
//     normal.normalized();
//     double intensity = std::max(0.0, normal * light_dir);
//     color = model->diffuse(uv) * intensity;
//     return false;
//   }
// private:
//   Vec2 uvs[3];
//   Vec3 normals[3];
//   Vec3 triangle[3];
// };

class DepthShader: public render::IShader {
public:
  virtual Vec4 vertex(int face, int nthvert) {
    Vec4 vec = render::local2homo(model->node(face, nthvert));
     return render::viewportmat * render::projectionmat * render::viewmat * render::modelmat* vec;
  }
  virtual bool fragment(Vec3 vec, render::TGAColor& color)  {
    color = render::TGAColor{255, 255, 255, 255};
    return false;
  }
private:

};

class ShadowShader: public render::IShader {
public:
  virtual Vec4 vertex(int face, int nthvert) {
      Vec4 node = render::local2homo(model->node(face, nthvert));
      triangles[nthvert] = node;
      uvs[nthvert] = model->uv(face , nthvert);
      return render::viewportmat * render::projectionmat * render::viewmat * render::modelmat* node;
  }
  virtual bool fragment(Vec3 vec, render::TGAColor& color)  {
      Vec2 uv = uvs[0] * vec[0] + uvs[1] * vec[1] + uvs[2] * vec[2];
      Vec3 n = render::homo2local(uniform_MIT * render::local2homo(model->normal(uv))).normalized();
      Vec3 l = render::homo2local(uniform_M * render::local2homo(light_dir)).normalized();
      // 反射光线
      Vec3 r = (2 * n * (n* l) - l).normalized();
      double diff = std::max(0.0, n * l);
      double specular = pow(std::max(r.z(), 0.), model->specular(uv));

      Vec4 light_Vec = mvpLight_ * triangles[0] * vec[0] + mvpLight_ * triangles[1] * vec[1] + mvpLight_ * triangles[2] * vec[2];
      light_Vec =  light_Vec / light_Vec[3];
      double depth = light_Vec.z();
      render::TGAColor cl = model->diffuse(uv);
      color = cl;
      for(int i = 0; i < 3; ++i) {
        if(depth  > buffer.get(vec.x(), vec.y()).bgra[0]) {
          color[i] = 0;
        } else {
          color[i] = std::min(255.0, 5.0 + cl[i] * (diff + 6 * specular));
        }
      }
      return false;
  }
 void init(Mat4 mvp_light, render::TGAImage& buffer_) {
      uniform_M =  render::viewmat * render::modelmat;
      uniform_MIT = (render::projectionmat * uniform_M).invert().transpose();
      buffer = buffer_;
      mvpLight_ = mvp_light;
  }
private:
  Vec4 triangles[3];
  Vec3 intensity_;
  Vec2 uvs[3];
  Mat4 uniform_M;
  Mat4 uniform_MIT;
  render::TGAImage buffer;
  Mat4 mvpLight_;
};

int main() {
  render::Window window;
  window.draw();
}





// int main() {
//   // test();

//   model = new render::Model("../obj/african_head.obj");
//   // render::modelmatrix(0);
//   // render::viewMatrix(light_dir, center, up);
//   // render::projectionMatrix(-1.f/(light_dir - center).norm());
//   // render::viewPortMatrix(width * 3 / 4, height * 3 / 4);

//   // Mat4 mvp_light = render::viewportmat * render::projectionmat * render::viewmat * render::modelmat;

//   // render::modelmat.clear();
//   // render::viewmat.clear();
//   // render::projectionmat.clear();
//   // render::viewportmat.clear();

//   render::modelmatrix(0);
//   render::viewMatrix(eye, center, up);
//   render::projectionMatrix(-1.f/(eye-center).norm());
//   render::viewPortMatrix(width * 3 / 4, height * 3 / 4);

//   light_dir = light_dir.normalized();

//   render::TGAImage image(width, height, 3);
//   render::TGAImage zbuffer(width, height, 1);
//   render::PhongShader shader(model);

//   // std::cout <<"ssssssss\n";
//   for (int i = 0; i < model->nfaces(); i++) { 
//     render::Vec4 screen_coords[3]; 
//     for (int j = 0; j < 3; j++) { 
//       screen_coords[j] = shader.vertex(i, j);
//     } 
//     triangle(screen_coords, shader, image, zbuffer); 
//     // os << model->node(i, 0) << "\n" << model->node(i, 1) << "\n" << model->node(i, 2) << "\n";
//   }

//   // render::TGAImage image1(width, height, 3);
//   // render::TGAImage zbuffer1(width, height, 1);
//   // ShadowShader shader1;
//   // shader1.init(mvp_light, zbuffer);
//   // shader1.init();
//   // for (int i = 0; i < model->nfaces(); i++) { 
//   //   render::Vec4 screen_coords[3]; 
//   //   for (int j = 0; j < 3; j++) { 
//   //     screen_coords[j] = shader1.vertex(i, j);
//   //   } 
//   //   triangle(screen_coords, shader1, image1, zbuffer1); 
//   //   // os << model->node(i, 0) << "\n" << model->node(i, 1) << "\n" << model->node(i, 2) << "\n";
//   // }
//   // test(image);
//   image.write_tga_file("example.tga");
//   zbuffer.write_tga_file("example.depth.tga");
//   delete model;


//   return 0;
// }

// int main() {
//   std::cout << "===============" << '\n';
//   glfwInit();
//     GLFWwindow* window;

//     // /* Initialize the library */
//     // if (!glfwInit())
//     //     return -1;

//     // /* Create a windowed mode window and its OpenGL context */
//     window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
//     // if (!window)
//     // {
//     //     glfwTerminate();
//     //     std::cout << " Create window error" << "\n";
//     //     return -1;
//     // }

//     // /* Make the window's context current */
//     // glfwMakeContextCurrent(window);

//     // /* Loop until the user closes the window */
//     while (!glfwWindowShouldClose(window))
//     {
//         /* Render here */
//         // glClear(GL_COLOR_BUFFER_BIT);
//         // glbegin();
//         /* Swap front and back buffers */
//         glfwSwapBuffers(window);

//         /* Poll for and process events */
//         glfwPollEvents();
//     }

//     glfwTerminate();
//   return 0;
// }