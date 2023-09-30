#include "Render.h"
#include "Engine/Model/model.h"
#include "Engine/Render.h"
#include "Engine/camera/camera.h"
#include "maths/Matrix.hpp"
#include "maths/maths.hpp"
#include "util.h"
#include <cmath>
#include "GLFW/glfw3.h"
#include <gl/GL.h>
#include <sstream>
#include "nlohmann/json.hpp"

namespace render {
static Camera* camera = Camera::GetCamera();
Mat4 modelMatrix(double angle);
Mat4 pespectiveMatrix(double eye_fov, double aspect_ratio, double zNear, double zFar);
Mat4 viewPortMatrix(double widthm, double height);
// 求出重心坐标
Vec3 barycentric(Vec4* vecs, Vec3 p);

// IShader
void IShader::setUniform(const std::string& name, const Mat4& mat) {
    map_[name] = mat;
}

// Render
Render::Render(): image_(width, height, 3) { zbuffer_.resize(width * height, 0); }

void Render::loadShader(IShader* shader) {
    shader_.reset(shader);
}

void Render::loadModel(const std::string& path) {
    model_.push_back(new Model(path));
}

void Render::triangle(Vec4 clip_coords[3]) {
    Vec4 screen_coords[3];
    for (int j = 0; j < 3; j++) { 
        screen_coords[j] = viewPortMatrix(width, height) * (clip_coords[j] /  clip_coords[j][3]);
    }
    // 寻找bounding box
    int bottom = std::floor(std::min(std::min(screen_coords[0].y(), screen_coords[1].y()), screen_coords[2].y()));
    int up = std::ceil(std::max(std::max(screen_coords[0].y(), screen_coords[1].y()), screen_coords[2].y()));
    int left = std::floor(std::min(std::min(screen_coords[0].x(), screen_coords[1].x()), screen_coords[2].x()));
    int right = std::ceil(std::max(std::max(screen_coords[0].x(), screen_coords[1].x()), screen_coords[2].x()));
    glBegin(GL_POINTS);
    for(int i = left; i <= right; ++i) {
        for(int j = bottom; j <= up; ++j) {
            Vec3 vec = barycentric(screen_coords, {i, j, 0});
            if(vec.x() < 0 || vec.y() < 0 || vec.z() <= 0) {
                continue;
            }
            vec[0] /= clip_coords[0].z();
            vec[1] /= clip_coords[1].z();
            vec[2] /= clip_coords[2].z();
            // double z = 0, w = 0;
            // 透视矫正 https://zhuanlan.zhihu.com/p/144331875
            double z = 1.0 / (vec[0] + vec[1] + vec[2]);
            // for(int t = 0; t < 3; ++t)  { 
            //     z += screen_coords[t].z() * vec[t];
            //     w += screen_coords[t][3] * vec[t];
            // }
            double frag_depth = z;
            if(zbuffer_[i + j * width] == 0 || frag_depth < zbuffer_[i + j * width]) {
                // std::cout << frag_depth << '\n';
                zbuffer_[i + j * width] = frag_depth;
                // zbuffer_.set(i,j, TGAColor{(uint8_t)(frag_depth), (uint8_t)frag_depth, (uint8_t)(frag_depth), 255});
                TGAColor color;
                bool flag = shader_->fragment(vec * z, color);
                if(!flag) {
                    image_.set(i, j, color);
                    // TODO
                    glColor3f(1.0f * color[2] / 255.0f, 1.0 * color[1] / 255.0f, 1.0 * color[0] / 255.0f);
                    glVertex3f( 1.0 * i / width * 2.0 - 1. , 1.0 * j / height * 2.0 - 1., 0.0);
                }
            }
        }
    }
    glEnd();
}
void Render::render() {;
    // view矩阵
    Mat4 view_matrix = camera->getViewMatrix();
    // 投影矩阵
    Mat4 projection_matrix = pespectiveMatrix(camera->zoom_, 1.0 * width / height, 0.1, 100.0);
    shader_->setUniform("view", view_matrix);
    shader_->setUniform("projection", projection_matrix);

    for(auto model : model_) {
        // 设置模型
        shader_->setModel(model);
        // 设置矩阵
        shader_->setUniform("model", model->model());
        for (int i = 0; i < model->nfaces(); i++) { 
            render::Vec4 clip_coords[3]; 
            for (int j = 0; j < 3; j++) { 
                clip_coords[j] = shader_->vertex(i, j);
                // 齐次坐标（主义和归一化normalized区分）
                // screen_coords[j] /= screen_coords[j][3];
                // screen_coords[j] =  viewPortMatrix(width, height) * screen_coords[j];
            } 
            triangle(clip_coords); 
        }
    }

}

// 采用unity shader入门精要里的，因为观察空间为右手系，到投影矩阵变换到左手系
Mat4 pespectiveMatrix(double eye_fov, double aspect_ratio, double zNear, double zFar) {
    double angle = eye_fov / 2 * std::numbers::pi / 180;
    Mat4 mat;
    double h = std::tan(angle) * zNear;
    double l =  h * aspect_ratio;
    mat <<  zNear / l, 0, 0, 0,
        0,  zNear / h, 0, 0,
        0, 0, (zNear + zFar) / (zNear - zFar), -2 * zNear * zFar / (zFar - zNear),
        0, 0, -1, 0;
    return mat;
}

Mat4 viewPortMatrix(double width, double height) {
    Mat4 mat;
    double w_ = width / 2.0;
    double h_ = height / 2.0;
    double z_ = depth / 2.0;
    mat << w_, 0, 0,  w_,
            0, h_, 0, h_,
            0, 0, z_, z_,
            0, 0, 0, 1;
    return mat;
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

}