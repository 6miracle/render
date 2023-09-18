#include "Render.h"
#include "Engine/Render.h"
#include "Engine/camera/camera.h"
#include "Engine/gl.h"
#include "maths/Matrix.hpp"
#include "maths/maths.hpp"
#include "util.h"
#include <cmath>

namespace render {
extern Camera camera;
Mat4 modelMatrix(double angle);
Mat4 pespectiveMatrix(double eye_fov, double aspect_ratio, double zNear, double zFar);

Render::Render(const std::string& path):model_(path) {}
void Render::loadShader(IShader* shader) {
    shader_.reset(shader);
}
void Render::render() {
    // model 矩阵
    Mat4 model = modelMatrix(0.0);
    // view矩阵
    Mat4 view = camera.getViewMatrix();
    // 投影矩阵
    Mat4  projection = pespectiveMatrix(45.0, 1.0 * width / height, 0.1, 100.0);
    // 透视除法
    
    // viewPort映射到屏幕坐标

}

// 平移矩阵
Mat4 translate(Vec3 vec) {
    Mat4 mat;
    mat << 1, 0, 0, vec.x(),
          0, 1, 0, vec.y(),
          0, 0, 1, vec.z(),
          0, 0, 0, 1;
    return mat;
}
// 旋转矩阵 (默认绕y轴转)
Mat4 rotate(double angle) {
    Mat4 mat;
    mat << std::cos(angle), 0, std::sin(angle), 0,
        0, 1, 0, 0,
        -std::sin(angle), 0, std::cos(angle), 0,
        0, 0, 0, 1;
    return mat;
}
// 缩放矩阵 
Mat4 scale(Vec3 vec) {
    Mat4 mat;
    mat << vec.x(), 0, 0,0,
            0, vec.y(), 0, 0,
            0, 0, vec.z(), 0,
            0, 0, 0, 1;
    return mat;
}
Mat4 modelMatrix(double angle) {
    // 缩放
    Mat4 scale;
    scale << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    // 旋转
    Mat4 rotation;
    rotation << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
    // 平移
    Mat4 translate;
    translate << 1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1;
    return  translate * rotation * scale;
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

}