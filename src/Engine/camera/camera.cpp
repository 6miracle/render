#include "camera.h"
#include "Engine/camera/camera.h"
#include "util.h"
#include <sstream>

namespace render {
Camera* Camera::camera_ = nullptr;
static Mat4 lookAt(Vec3 position, Vec3 center, Vec3 up) {
    Vec3 zaxis = (position - center).normalized();
    Vec3 xaxis = (up.normalized().cross(zaxis)).normalized();
    Vec3 yaxis = (zaxis.cross(xaxis)).normalized();
    Mat4 modelmat;
    Mat4 viewmat;
    modelmat << xaxis.x(), xaxis.y(), xaxis.z(), 0,
        yaxis.x(), yaxis.y(), yaxis.z(), 0,
        zaxis.x(), zaxis.y(), zaxis.z(), 0,
        0, 0, 0, 1;
    viewmat << 1, 0, 0, -position.x(),
            0, 1, 0, -position.y(),
            0, 0, 1, -position.z(),
            0, 0, 0, 1;
    return modelmat * viewmat;
}


double radians(double angle) {
   return angle / 180.0 * std::numbers::pi;
}   

// =========================Camera==========================
Camera* Camera::GetCamera(CameraType type, Vec3 position, Vec3 up, Vec3 target, double yaw, double pitch) {
     if(camera_ && camera_->type() == type) {
            return camera_;
        } else {
            Camera* tmp = camera_;
            switch (type) {
                case NONE:
                //  printf("asdsdsd\n");
                    camera_ = new Camera(position, up, yaw, pitch);
                    break;
                case FPS:
                // printf("//////////\n");
                    camera_ = new FPSCamera(position, up, yaw, pitch);
                    break;
                case SURROUND:
                // printf("3322\n");
                    camera_ = new OrbitCamera();
                    break;
            }
            delete tmp;
            return camera_;
        }
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
     return lookAt(position_, position_ + front_, worldup_);
}
void Camera::updateCameraVectors() {
    Vec3 front;
    front[0] = std::cos(radians(yaw_)) * std::cos(radians(pitch_));   // 0
    front[1] = std::sin(radians(pitch_));     // 0
    front[2] = std::sin(radians(yaw_)) * std::cos(radians(pitch_)); // -1

    front_ = front.normalized();
    right_ = front.cross(worldup_).normalized();
    up_ = right_.cross(front_).normalized();
}
void Camera::ProcessKeyboard(Camera_Movement direction, double deltaTime) {}
void Camera::processMouseMovement(double xoffset, double yoffset, bool constrainPitch) {}
void Camera::ProcessMouseScroll(double yoffset) {};
// ================================FPSCamera============================
FPSCamera::FPSCamera(Vec3 position, Vec3 up, double yaw, double pitch):Camera(position, up, yaw, pitch) {
    type_ = FPS;
}
FPSCamera::FPSCamera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw, double pitch):
    Camera(posX, posY, posZ, upX, upY, upZ, yaw, pitch) {
        type_ = FPS;
    }
void FPSCamera::ProcessKeyboard(Camera_Movement direction, double deltaTime) {
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
void FPSCamera::processMouseMovement(double xoffset, double yoffset, bool constrainPitch) {
        xoffset *= sensiticity_;
        yoffset *= sensiticity_;

        yaw_ += xoffset;
        pitch_ += yoffset;

        if(constrainPitch) {
            if(pitch_ > 89.0) {
                pitch_ = 89.0;
            } else if(pitch_ < -89.0) {
                pitch_ = -89.0;
            }
        }
        updateCameraVectors();
    }

void FPSCamera::ProcessMouseScroll(double yoffset) {
    zoom_ -= yoffset;
    if(zoom_ < 1.0f) {
        zoom_ = 1.0f;
    } else if(zoom_ > 45.0f) {
        zoom_ = 45.0f;
    }
}

// =================OribitCamera================
OrbitCamera::OrbitCamera(Vec3 position, Vec3 up, Vec3 target, double yaw, double pitch)
    :Camera(position, up, yaw, pitch), target_(target) { 
        type_ = SURROUND;
        updateCameraVectors();
        // std::cout << ToString() << '\n';
    }
OrbitCamera::OrbitCamera(double posX, double posY, double posZ, double upX, double upY, double upZ, 
    double targetX, double targetY, double targetZ, double yaw, double pitch) 
    :Camera(posX, posY, posZ, upX, upY, upZ, yaw, pitch), target_(Vec3{targetX, targetY, targetZ}) {
        type_ = SURROUND;
        updateCameraVectors();
}

void OrbitCamera::ProcessKeyboard(Camera_Movement direction, double deltaTime)  {
    double velocity = speed_ * deltaTime;
    switch(direction){
        case FORWARD:
            target_ += front_ * velocity;
            break;
        case BACKWORD:
            target_ -= front_ * velocity;
            break;
        case LEFT:
            yaw_ -= 1;
            updateCameraVectors();
            break;
        case RIGHT:
            yaw_ += 1;
            updateCameraVectors();
            break;
    }
}
void OrbitCamera::processMouseMovement(double xoffset, double yoffset, bool constrainPitch) {
    xoffset *= sensiticity_;
    yoffset *= sensiticity_;

    yaw_ -= xoffset;
    pitch_ -= yoffset;

    if(constrainPitch) {
        if(pitch_ > 89.0f) {
            pitch_ = 89.0f;
        } else if(pitch_ < -89.0f) {
            pitch_ = -89.0f;
        }
    }
    updateCameraVectors();
}
void OrbitCamera::ProcessMouseScroll(double yoffset) {
    zoom_ -= yoffset;
    if(zoom_ < 1.0f) {
        zoom_ = 1.0f;
    } else if(zoom_ > 89.0f) {
        zoom_ = 89.0f;
    }
}

void OrbitCamera::updateCameraVectors() {
    // std::cout << "Orbit " << '\n';
    Vec3 front;
    front[0] = std::cos(radians(yaw_)) * std::cos(radians(pitch_));   // 0
    front[1] = std::sin(radians(pitch_));         // 0
    front[2] = std::sin(radians(yaw_)) * std::cos(radians(pitch_));   // 1
    // std::cout << " === = = = = = \n";
    front_ =  -1 * front.normalized();
    right_ = front.cross(worldup_).normalized();
    up_ = right_.cross(front_).normalized();
}
Mat4 OrbitCamera::getViewMatrix() {
    // std::cout << "front = "  << front_ << '\n';
    // std::cout << "target = "  << target_ << '\n';
    return lookAt(target_ - front_, target_, worldup_);
}
// // ============================OrbitCamera====================
// OribitCamera::OribitCamera(Vec3 position, Vec3 up , Vec3 target) {

// }
// void OribitCamera::ProcessKeyboard(Camera_Movement direction, double deltaTime) {
    
// }
// void OribitCamera::processMouseMovement(double xoffset, double yoffset, bool constrainPitch) {
//     xoffset *= sensiticity_;
//     yoffset *= sensiticity_;

//     yaw_ -= xoffset;
//     pitch_ -= yoffset;

//     if(constrainPitch) {
//         if(pitch_ > 89.0f) {
//             pitch_ = 89.0f;
//         } else if(pitch_ < -89.0f) {
//             pitch_ = -89.0f;
//         }
//     }
//     updateCameraVectors();
// }
// void OribitCamera::ProcessMouseScroll(double yoffset){
    
// }

// void OribitCamera::updateCameraVectors() {
//     Vec3 front;
//     front[0] = std::cos(radians(yaw_)) * std::cos(radians(pitch_));
//     front[1] = std::sin(radians(pitch_));
//     front[2] = std::sin(radians(yaw_)) * std::cos(radians(pitch_));

//     front_ = -1 * front.normalized();
//     right_ = front.cross(worldup_).normalized();
//     up_ = right_.cross(front_).normalized();
// }

// Mat4 OrbitCamera::getViewMatrix() {
//     return lookAt(target - front_, target, worldup_);
// }
}