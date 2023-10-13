#ifndef __RENDER_SCENE_H__
#define __RENDER_SCENE_H__

#include "tgaimage.h"
namespace render {
double DistributionGG(Vec3 N, Vec3 H, double roughness);
double GeometrySmith(Vec3 N, Vec3 V, Vec3 L, double roughness);
double fresnelSchlick(double cosTheta, double F0);
double fresnelSchlick_IBL(double cosTheta, double F0, double roughness);
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

    TGAColor color(Ray& ray);
private:


};

}
#endif