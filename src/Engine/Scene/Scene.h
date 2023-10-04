#ifndef __RENDER_SCENE_H__
#define __RENDER_SCENE_H__

#include "tgaimage.h"
namespace render {

// ray = a + tb
class Ray {
public:
    Ray(Vec3 origin, Vec3 direction): origin_(origin), direction_(direction) {}
    Vec3 at(double t) {
        return origin_ + direction_ * t;
    }
    Vec3 origin() const noexcept { return origin_; }
    Vec3 direction() const noexcept { return direction_; }
private:
    Vec3 origin_;
    Vec3 direction_;
};
class Scene {
public:

    TGAColor color();
private:


};

}
#endif