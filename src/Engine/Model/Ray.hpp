#ifndef __RENDER_RAY_HPP__
#define __RENDER_RAY_HPP__

namespace render {
class Ray {
public:
    Ray() = default;
    Ray(Vec3 origin, Vec3 direction): origin_(origin), direction_(direction), tm_(0.0) {}
    Ray(Vec3 origin, Vec3 direction, double tm): origin_(origin), direction_(direction), tm_(tm) {}
    Vec3 at(double t) const {
        return origin_ + direction_ * t;
    }
    Vec3 origin() const noexcept { return origin_; }
    Vec3 direction() const noexcept { return direction_; }
    double time() const noexcept { return tm_; }
    
private:
    Vec3 origin_;
    Vec3 direction_;
    double tm_; // 用于运动模糊
};
}

#endif