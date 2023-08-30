#include "maths/Matrix.hpp"
#include "maths/maths.hpp"
#include "model.h"
#include "tools/draw.hpp"
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
using render::Mat3;
using  render::Mat4;
const render::TGAColor white = render::TGAColor {.bgra = {255, 255, 255, 255}};
const render::TGAColor red   = render::TGAColor{255, 0,   0};
const render::TGAColor green = render::TGAColor{0,   255,  0,  255, 4};
const render::TGAColor blue = render::TGAColor{ .bgra = {0, 0, 255, 255} };


void test() {
  Mat3 mat;
  mat << 1 << 2 << 3 << 4 
    << 1 << 2 << 3 << 4 
    << 1;
  // std::cout << mat << "\n";
    // exit(0);

}

int main() {
  // test();
  // std::ofstream os("ex.txt");
  // render::TGAImage image(width, height, 3);

  // render::Model model("../obj/african_head.obj");
  // render::Vec3 light_dir{0 ,0, -1};

  // double *depth_buffer = new double[width * height];
  // for(size_t i = 0; i < width * height; ++i) {
  //   depth_buffer[i] = -std::numeric_limits<double>::max();
  // }

  // model.test(os);
  // for (int i = 0; i < model.nfaces(); i++) { 
  //   render::Vec3 screen_coords[3]; 
  //   render::Vec3 world_coords[3];
  //   for (int j = 0; j < 3; j++) { 
  //       render::Vec3 world_coord = model.node(i, j); 
  //       screen_coords[j] = render::world2screen(world_coord);
  //       world_coords[j] = world_coord;
  //   } 
  //   render::Vec3 n = (world_coords[2] - world_coords[0]).cross(world_coords[1] - world_coords[0]);
  //   n = n.normalized();
  //   double intensity = n * light_dir;
  //   Vec2 uvs[3]{model.uv(i, 0), model.uv(i, 1), model.uv(i, 2)};

  //   if(intensity > 0) {
  //     triangle(screen_coords, uvs, depth_buffer, image, intensity, model); 
  //   }
  // }

  // // test(image);
  // image.write_tga_file("example.tga");
  // delete [] depth_buffer;

   Mat3 mat;
   std::cout << mat <<"\n";
  mat << 1, 2, 3, 4, 5, 6, 7, 8, 9;
    std::cout << mat << "\n";
  return 0;
}