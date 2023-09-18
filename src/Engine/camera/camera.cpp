#include "camera.h"
#include "Engine/camera/camera.h"

namespace render {
static Mat4 lookAt(Vec3 position, Vec3 center, Vec3 up) {
    Vec3 zaxis = (center - position).normalized();
    Vec3 xaxis = (up.normalized().cross(zaxis)).normalized();
    Vec3 yaxis = (zaxis.cross(xaxis)).normalized();
    Mat4 mat;
    mat << xaxis.x(), xaxis.y(), xaxis.z(), -position.x(),
        yaxis.x(), yaxis.y(), yaxis.z(), -position.y(),
        zaxis.x(), zaxis.y(), zaxis.z(), -position.z(),
        0, 0, 0, 1;
    return mat;
}


double radians(double angle) {
   return angle / 180.0 * std::numbers::pi;
}   

Camera::Camera(Vec3 position, Vec3 up, double yaw, double pitch): position_(position), 
        front_(Vec3{0.0f, 0.0f, -0.1f}),
        worldup_(up),
        yaw_(yaw),
        pitch_(pitch),
        speed_(SPEED),
        sensiticity_(SENSITIVITY),
        zoom_(ZOOM) 
        {
            updateCameraVectors();
        }
Camera::Camera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw, double pitch):
        position_{posX, posY, posZ}, 
        front_(Vec3{0.0f, 0.0f, -0.1f}),
        worldup_{upX, upY, upZ},
        yaw_(yaw),
        pitch_(pitch) 
        {
            updateCameraVectors();
        }
Mat4 Camera::getViewMatrix() {
     return lookAt(position_, position_ + front_, up_);
}

void Camera::ProcessKeyboard(Camera_Movement direction, double deltaTime) {
    double velocity = speed_ * deltaTime;
    switch(direction){
        case FORWARD:
            position_ += front_ * velocity;
            break;
        case BACKWORD:
            position_ -= front_ * velocity;
            break;
        case LEFT:
            position_ -= right_ * velocity;
            break;
        case RIGHT:
            position_ += right_ * velocity;
            break;
    }
}
void Camera::processMouseMovement(double xoffset, double yoffset, bool constrainPitch) {
        xoffset *= sensiticity_;
        yoffset *= sensiticity_;

        yaw_ += xoffset;
        pitch_ += yoffset;

        if(constrainPitch) {
            if(pitch_ > 89.0f) {
                pitch_ = 89.0f;
            } else if(pitch_ < -89.0f) {
                pitch_ = -89.0f;
            }
        }
        updateCameraVectors();
    }

void Camera::ProcessMouseScroll(double yoffset) {
    zoom_ -= yoffset;
    if(zoom_ < 1.0f) {
        zoom_ = 1.0f;
    } else if(zoom_ > 45.0f) {
        zoom_ = 45.0f;
    }
}


void Camera::updateCameraVectors() {
    Vec3 front;
    front[0] = std::cos(radians(yaw_)) * std::cos(radians(pitch_));
    front[1] = std::sin(radians(pitch_));
    front[2] = std::sin(radians(yaw_)) * std::cos(radians(pitch_));

    front_ = front.normalized();
    right_ = front.cross(worldup_).normalized();
    up_ = right_.cross(front_).normalized();
}

}