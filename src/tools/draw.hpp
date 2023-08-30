#ifndef __RENDER_TOOLS_DRAW_HPP__
#define __RENDER_TOOLS_DRAW_HPP__

#include "maths/maths.hpp"
#include "model.h"
#include "tgaimage.h"
#include "util.h"
#include <cmath>
#include <exception>
#include <fstream>
#include <setjmp.h>
#include <stdint.h>
namespace render {

// breshman直线绘制算法
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool flag = false;
    if(std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        flag = true;
    }
    if(x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror = 2 * std::abs(dy);
    int error = 0;
    for(int x = x0, y = y0; x <= x1; ++x) {
        if(flag) image.set(y, x, color);
        else image.set(x, y , color);

        error += derror;
        if(error > dx) {
            y += y1 > y0 ? 1 : -1;
            error -= 2* dx;
        }
    }
}

void line(Vec2 v1, Vec2 v2, TGAImage& image, TGAColor color) {
    line(v1.x(), v1.y(), v2.x(), v2.y(), image, color);
}

// static int lll = 0;
// 三角形绘制

// 向量叉乘法1
bool inTriangle(Vec3 v1, Vec3 v2, Vec3 v3, int x, int y) {
    // if(!lll) { ++lll; std::cout << "inTriangle1(): "; }
    Vec2 v12 = {v2.x() - v1.x(), v2.y() - v1.y()};
    Vec2 v23 = {v3.x() - v2.x(), v3.y() - v2.y()};
    Vec2 v31 = {v1.x() - v3.x(), v1.y() - v3.y()};
    int val1 = v12.cross({x - v1.x(), y - v1.y()});
    int val2 = v23.cross({x - v2.x(), y - v2.y()});
    int val3 = v31.cross({x - v3.x(), y - v3.y()});
    return (val1 >= 0 && val2 >= 0 && val3 >= 0) || (val1 <= 0 && val2 <= 0 && val3 <= 0);
}
// 向量叉乘法2

bool inTriangle1(Vec3 v1, Vec3 v2, Vec3 v3, int x, int y) {
    // if(!lll) { ++lll; std::cout << "inTriangle2(pa, pb ,pc): "; }
    Vec2 pa{v1.x() - x, v1.y() - y };
    Vec2 pb{v2.x() - x, v2.y() - y};
    Vec2 pc{v3.x() - x, v3.y() - y};

    int val1 = pa.cross(pb);
    int val2 = pb.cross(pc);
    int val3 = pc.cross(pa);
    return (val1 >= 0 && val2 >= 0 && val3 >= 0) || (val1 <= 0 && val2 <= 0 && val3 <= 0);
}

// 求出重心坐标
Vec3 barycentric(Vec3* vecs, Vec3 p) {
    double xa = vecs[0].x();
    double xb = vecs[1].x();
    double xc = vecs[2].x();
    double ya = vecs[0].y();
    double yb = vecs[1].y();
    double yc = vecs[2].y();

    double gamma = ((ya - yb) * p.x() + (xb - xa) * p.y() + xa * yb - xb * ya) / ((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya);
    double beta = ((ya - yc) * p.x() + (xc - xa) * p.y() + xa * yc - xc * ya) / ((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya);
    double alpha = 1 - beta - gamma;
    return Vec3{alpha, beta, gamma};
}

bool inTriangle3(Vec3 v1, Vec3 v2, Vec3 v3, int x, int y) {
   
    Vec3 vecs[3] = {v1, v2, v3};
    Vec3 vec = barycentric(vecs, {x, y, 0});
    return vec.x() >= 0 && vec.y() >= 0 && vec.z() >= 0;
}



bool triangle(Vec3* vecs, Vec2* uvs,  double buffer[], TGAImage& image, double intensity, Model& model) {
    // 寻找bounding box
    int bottom = std::floor(std::min(std::min(vecs[0].y(), vecs[1].y()), vecs[2].y()));
    int up = std::ceil(std::max(std::max(vecs[0].y(), vecs[1].y()), vecs[2].y()));
    int left = std::floor(std::min(std::min(vecs[0].x(), vecs[1].x()), vecs[2].x()));
    int right = std::ceil(std::max(std::max(vecs[0].x(), vecs[1].x()), vecs[2].x()));

    for(int i = left; i <= right; ++i) {
        for(int j = bottom; j <= up; ++j) {
            Vec3 vec = barycentric(vecs, {i, j, 0});
            if(vec.x() < 0 || vec.y() < 0 || vec.z() < 0) {
                continue;
            }

            double z = 0;
            for(int t = 0; t < 3; ++t) z += vecs[t].z() * vec[t];

            if(buffer[(int)(i + j * width)] < z) {
                buffer[(int)(i + j * width)] = z;
                Vec2 uv = uvs[0] * vec.x() + uvs[1] * vec.y() + uvs[2] * vec.z();
                TGAColor color = model.diffuse().get(uv.x() * model.diffuse().width(), uv.y() * model.diffuse().height());
                image.set(i, j, TGAColor{(uint8_t)(color.bgra[0] * intensity), 
                    (uint8_t)(color.bgra[1]* intensity), (uint8_t)(color.bgra[2]*intensity), 255});  
            }
            //             image.set(i, j, render::TGAColor {.bgra = {static_cast<uint8_t>(intensity * 255), 
            //         static_cast<uint8_t>(intensity * 255), static_cast<uint8_t>(intensity * 255), 255}});
            //  if(inTriangle3(vecs[0], vecs[1], vecs[2], i, j)) {
            //         image.set(i, j, render::TGAColor {.bgra = {static_cast<uint8_t>(intensity * 255), 
            //         static_cast<uint8_t>(intensity * 255), static_cast<uint8_t>(intensity * 255), 255}});
            //     }
        }
    }
    // os.close();
    return true;
}

Vec3 world2screen(Vec3 vec) {
    return { ((vec.x() + 1.) * width /2) + .5, ((vec.y() + 1.) * height /2) + .5, vec.z()};
}
}

#endif