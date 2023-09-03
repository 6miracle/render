#include "gl.h"
#include "GL/gl.h"
#include "maths/Matrix.hpp"
#include "maths/maths.hpp"
#include "tgaimage.h"
#include "util.h"
#include <exception>
#include <fstream>
#include <stdint.h>


namespace render {
Mat4 modelmat;
Mat4 viewmat;
Mat4 viewportmat;
Mat4 projectionmat;

IShader::~IShader() {}
// void modelmatrix(Vec3 eye, Vec3 center, Vec3 up) {
//     Vec3 z = (center - eye).normalized();
//     Vec3 x = up.cross(z).normalized();
//     Vec3 y = z.cross(x).normalized(); 
//     modelmat << localvhomo(x) << localvhomo(y) << localvhomo(z);
// }

// 绕z轴旋转
void modelmatrix(double angle) {
    Mat4 scale;
    scale << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    Mat4 rotation;
    rotation << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
    Mat4 translate;
    translate << 1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1;
    modelmat = translate * rotation * scale;
}

void viewMatrix(Vec3 eye, Vec3 center, Vec3 up) {
     Vec3 z = (eye - center).normalized();
    Vec3 x = up.cross(z).normalized();
    Vec3 y = z.cross(x).normalized(); 
    viewmat << localvhomo(x) << localvhomo(y) << localvhomo(z);
    viewmat << 0, 0, 0, 1;
    viewmat[0][3] = -center[0];
    viewmat[1][3] = -center[1];
    viewmat[2][3] = -center[2];
}

// 垂直可视角度 宽高比 相机与近平面距离 相机与远平面距离
void projectionMatrix(double eye_fov, double aspect_ratio, double  zNear, double zFar) {
   
    double angle = eye_fov / 2 * std::numbers::pi / 180;

    double h = std::tan(angle) * zNear;
    double l =  h * aspect_ratio;
    // zNear = -zNear;
    // zFar = -zFar;
    projectionmat <<  zNear / l, 0, 0, 0,
        0,  zNear / h, 0, 0,
        0, 0, (zNear + zFar) / (zNear - zFar), 2 * zNear * zFar / (zFar - zNear),
        0, 0, 1, 0;
}

void projectionMatrix(double val) {
    projectionmat << 1, 0, 0, 0, 
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, val, 1;
}
void viewPortMatrix( int w, int h) {
    // std::cout << depth << "\n";
    double w_ = w / 2.0;
    double h_ = h / 2.0;
    double z_ = depth / 2.0;
    viewportmat << w_, 0, 0,  w_,
            0, h_, 0, h_,
            0, 0, z_, z_,
            0, 0, 0, 1;
}


// 求出重心坐标
Vec3 barycentric(Vec4* vecs, Vec3 p) {
    double xa = vecs[0].x();
    double xb = vecs[1].x();
    double xc = vecs[2].x();
    double ya = vecs[0].y();
    double yb = vecs[1].y();
    double yc = vecs[2].y();

    double gamma = ((ya - yb) * p.x() + (xb - xa) * p.y() + xa * yb - xb * ya) / ((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya);
    if(std::abs(gamma) < 1e-5) { return Vec3{-1, 1, 1}; } 
    double beta = ((ya - yc) * p.x() + (xc - xa) * p.y() + xa * yc - xc * ya) / ((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya);
    double alpha = 1 - beta - gamma;
    return Vec3{alpha, beta, gamma};


}

void triangle(Vec4 *vecs, IShader &shader, TGAImage &image, TGAImage &zbuffer) {
    vecs[0] = vecs[0] / vecs[0][3];
    vecs[1] = vecs[1] / vecs[1][3];
    vecs[2] = vecs[2] / vecs[2][3];
    // 寻找bounding box
    int bottom = std::floor(std::min(std::min(vecs[0].y(), vecs[1].y()), vecs[2].y()));
    int up = std::ceil(std::max(std::max(vecs[0].y(), vecs[1].y()), vecs[2].y()));
    int left = std::floor(std::min(std::min(vecs[0].x(), vecs[1].x()), vecs[2].x()));
    int right = std::ceil(std::max(std::max(vecs[0].x(), vecs[1].x()), vecs[2].x()));

    for(int i = left; i <= right; ++i) {
        for(int j = bottom; j <= up; ++j) {
            // Vec4 tmp[3] = {vecs[0] / vecs[0][3], vecs[1] / vecs[1][3], vecs[2] / vecs[2][3]};
            Vec3 vec = barycentric(vecs, {i, j, 0});
            if(vec.x() < 0 || vec.y() < 0 || vec.z() < 0) {
                continue;
            }
            // os << vec<< "\n";
            double z = 0 ,w = 0;
            for(int t = 0; t < 3; ++t)  { 
                z += vecs[t].z() * vec[t];
                w += vecs[t][3] * vec[t];
            }
            int frag_depth = std::max(0, std::min(255, int(z/w+.5)));
            if(frag_depth > zbuffer.get(i, j).bgra[0]) {
                zbuffer.set(i,j, TGAColor{(uint8_t)(frag_depth), (uint8_t)frag_depth, (uint8_t)(frag_depth), 255});
                TGAColor color;
                shader.fragment(vec, color);
                image.set(i, j, color);
            }
        }
    }
}
}