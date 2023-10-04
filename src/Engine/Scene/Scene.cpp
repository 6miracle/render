#include "Scene.h"
#include "Engine/Scene/Scene.h"
#include "tgaimage.h"

namespace render {
bool hitSphere(const Vec3& center, double radius, const Ray& ray) {
    Vec3 ac = ray.direction() - center;
    double b = 2.0 * ac * ray.direction();
    double a = ray.direction() * ray.direction();
    double c = ac * ac - radius * radius;
    return (b * b - 4 * a * c ) >= 0;
}


TGAColor Scene::color() {
    Ray ray(Vec3{0, 0, 0}, Vec3{0, 0, 1});
    if(!hitSphere(Vec3{0, 0, 1}, 100, ray)) {
        return TGAColor{0, 0, 0, 0};
    }
    return TGAColor{255, 255,255,255};
}
}