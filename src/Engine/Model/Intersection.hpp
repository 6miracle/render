#ifndef __RENDER__INTERSECTION__HPP__
#define __RENDER__INTERSECTION__HPP__

#include "Engine/Model/Ray.hpp"
#include <memory>
namespace render {
class Material;
class Intersection {
public:
    Vec3 p;  // 交点
    Vec3 normal;
    std::shared_ptr<Material> mat;
    Vec2 uv;  // 交点处的纹理坐标
    double t;
    bool front; // ray is outside the sphere

    // 让法线方向总是与光线方向相反
    void setFaceNormal(const Ray& ray, const Vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.
        front = (ray.direction() * outward_normal) < 0;  // 光线从外面射入
        normal = front ? outward_normal : -1 * outward_normal;
    }
};
}
#endif