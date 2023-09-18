#ifndef __RENDER_CAMERA_H__
#define __RENDER_CAMERA_H__

#include "pch.h"
#include "maths/maths.hpp"
#include "maths/Matrix.hpp"
namespace render {
enum Camera_Movement {
    FORWARD,
    BACKWORD,
    LEFT,
    RIGHT
};

const double YAW = -90.;  // 偏航角
const double PITCH = 0.0;  // 俯仰角
const double SPEED = 2.5;
const double SENSITIVITY = 0.1;
const double ZOOM = 45.0;

class Camera {
public:
    // 相机属性
    Vec3 position_;
    Vec3 front_;
    Vec3 up_;
    Vec3 right_;
    Vec3 worldup_;

    // 欧拉角
    double yaw_;
    double pitch_;
    // 相机可选项
    double speed_; // 鼠标移动速度
    double sensiticity_; // 鼠标灵敏度
    double zoom_; // 缩放级别(视野值)

    Camera(Vec3 position = Vec3{0.0, 0.0, 0.0}, Vec3 up = Vec3{0.0, 1.0, 0.0}, 
        double yaw = YAW, double pitch = PITCH);
    Camera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw = YAW, double pitch = PITCH);
    Mat4 getViewMatrix();
    void ProcessKeyboard(Camera_Movement direction, double deltaTime);

    void processMouseMovement(double xoffset, double yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(double yoffset);
private:
    void updateCameraVectors();
};

}
#endif