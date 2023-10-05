#include "Scene.h"
#include "Engine/Scene/Scene.h"
#include "tgaimage.h"

namespace render {
double hitSphere(const Vec3& center, double radius, const Ray& ray) {
    Vec3 ac = ray.origin() - center;
    double half_b = ac * ray.direction();
    double a = ray.direction() * ray.direction();
    double c = ac * ac - radius * radius;
   double dis = half_b * half_b -  a * c ;
   if(dis < 0) { return -1.0; }
   else {
    return (-half_b - std::sqrt(dis)) / a;
   }
}


TGAColor Scene::color( Ray& r) {
    double res = hitSphere(Vec3{0.5, 0.5, -1.0f}, 0.5, r);
    if(res >= 0) {
        Vec3 n = (r.at(res) - Vec3{0.0, 0.0, -1.0f}).normalized();
        return TGAColor{static_cast<unsigned char>(255 * 0.5 *(n.x() + 1)), 
            static_cast<unsigned char>(255 *0.5 * (n.y() + 1)), static_cast<unsigned char>(255 *0.5 * (n.z() + 1)) };
    }
    
    Vec3 unit_direction = r.direction().normalized();
    auto a = 0.5*(unit_direction.y() + 1.0);
    return TGAColor{255, 255, 255} *  (1.0-a) + TGAColor{(uint8_t)(255 * 0.5), (uint8_t)(255 * 0.7), 255} * a;
    // return TGAColor{255, 255,255,255};
    // return TGAColor{255, 255}
}
}