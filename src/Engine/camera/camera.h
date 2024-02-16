#ifndef __RENDER_CAMERA_H__
#define __RENDER_CAMERA_H__

#include "pch.h"
#include "maths/maths.hpp"
#include "maths/Matrix.hpp"
#include <sstream>
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
    enum CameraType{
        NONE,
        FPS,
        SURROUND
    };
    // 相机属性
    Vec3 position_;
    Vec3 front_;
    Vec3 up_;
    Vec3 right_;
    Vec3 worldup_;
    Vec3 target_;

    // 欧拉角
    double yaw_;   // 偏航角
    double pitch_; // 俯仰角
    // 相机可选项
    double speed_; // 鼠标移动速度
    double sensiticity_; // 鼠标灵敏度
    double zoom_; // 缩放级别(视野值)

    // 用于tracer
    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10.0;    // Distance from camera lookfrom point to plane of perfect focus
public:
    virtual Mat4 getViewMatrix();
    virtual void updateCameraVectors();
    virtual void ProcessKeyboard(Camera_Movement direction, double deltaTime);

    virtual void processMouseMovement(double xoffset, double yoffset, bool constrainPitch = true);
    virtual void ProcessMouseScroll(double yoffset);
    double focalLength();
    std::pair<double,double> ViewportLength();

    CameraType type() { return type_; }
    static Camera* GetCamera(CameraType type = FPS, Vec3 position = Vec3{0.0, 0.0, 3.0}, Vec3 up = Vec3{0.0, 1.0, 0.0}, 
        Vec3 target = Vec3{0, 0, 2}, double yaw = YAW, double pitch = PITCH);
protected:
    Camera(Vec3 position = Vec3{0.0, 0.0, 0.0}, Vec3 up = Vec3{0.0, 1.0, 0.0}, 
        double yaw = YAW, double pitch = PITCH);
    Camera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw = YAW, double pitch = PITCH);
    CameraType type_;
private:
    static Camera* camera_;
};

class FPSCamera: public Camera {
public:
    FPSCamera(Vec3 position = Vec3{0.0, 0.0, 0.0}, Vec3 up = Vec3{0.0, 1.0, 0.0}, 
        double yaw = YAW, double pitch = PITCH);
    FPSCamera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw = YAW, double pitch = PITCH);
    void ProcessKeyboard(Camera_Movement direction, double deltaTime) override;

    void processMouseMovement(double xoffset, double yoffset, bool constrainPitch = true) override;
    void ProcessMouseScroll(double yoffset) override;
};
class OrbitCamera:public Camera{
public:
    OrbitCamera(Vec3 position = Vec3{0.0, 0.0, 3.0}, Vec3 up = Vec3{0.0, 1.0, 0.0}, 
        Vec3 target = Vec3 {0, 0, 3}, double yaw = -YAW, double pitch = PITCH);
    OrbitCamera(double posX, double posY, double posZ, double upX, double upY, double upZ, double targrtX, 
        double targetY, double targetZ, double yaw = -YAW, double pitch = PITCH);

    void ProcessKeyboard(Camera_Movement direction, double deltaTime) override;
    void processMouseMovement(double xoffset, double yoffset, bool constrainPitch = true) override;
    void ProcessMouseScroll(double yoffset) override;
    Mat4 getViewMatrix() override;
    void updateCameraVectors() override;
private:
    Vec3 target_;

    std::string ToString() const{ 
        std::stringstream ss; 
        ss << "position = " << position_ << "   target = " << target_ << " front = " << front_ ;
        return ss.str();
     }
};


// class OrbitCamera {
// public:
//     OrbitCamera(Vec3 position = Vec3{0.0, 0.0, 3.0}, Vec3 up = Vec3{0.0, 1.0, 0.0}, Vec3 target = Vec3 {0, 0, 2});
//     void ProcessKeyboard(Camera_Movement direction, double deltaTime);
//     void processMouseMovement(double xoffset, double yoffset, bool constrainPitch = true);
//     void ProcessMouseScroll(double yoffset);
//     void updateCameraVectors();
//     Mat4 getViewMatrix();
// private:
//     Vec3 position_;
//     Vec3 front_;
//     Vec3 up_;
//     Vec3 right_;
//     Vec3 worldup_;
//     Vec3 target;

//     // 欧拉角
//     double yaw_;   // 偏航角
//     double pitch_; // 俯仰角
//     // 相机可选项
//     double speed_; // 鼠标移动速度
//     double sensiticity_; // 鼠标灵敏度
//     double zoom_; // 缩放级别(视野值)
// };
}
#endif