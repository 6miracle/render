#ifndef __RENDER_AABB_HPP__
#define __RENDER_AABB_HPP__

#include "Engine/Model/Ray.hpp"
#include "pch.h"
// #include <corecrt_wtime.h>
#include <limits>

namespace render {
// AABB包围盒
class Bounds {
public:
    Bounds() {
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        pMin = Vec3{maxNum, maxNum, maxNum};
        pMax = Vec3{minNum, minNum, minNum};
    }
    Bounds(const Vec3& p): pMin(p), pMax(p) {}
    Bounds(const Vec3& p1, const Vec3& p2) {
        pMin = Vec3{std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()), std::min(p1.z(), p2.z())};
        pMax = Vec3{std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y()), std::max(p1.z(), p2.z())};
    }
    Bounds(const Bounds& b1, const Bounds& b2) {
        pMin = Vec3{std::min(b1.Min().x(), b2.Min().x()), 
            std::min(b1.Min().y(), b2.Min().y()), std::min(b1.Min().z(), b2.Min().z())};
        pMax = Vec3{std::max(b1.Max().x(), b2.Max().x()), 
            std::max(b1.Max().y(), b2.Max().y()), std::max(b1.Max().z(), b2.Max().z())};
    }

    Bounds(const Bounds& b) {
        pMin = b.pMin;
        pMax = b.pMax;
    }
    Bounds& operator=(const Bounds& b) {
        pMin = b.pMin;
        pMax = b.pMax;
        return *this;
    }

    Vec3 Min() const { return pMin; }
    Vec3 Max() const { return pMax; }
    // bool hit(const Ray& ray, double ray_tmin, double ray_tmax) const {
    //     Vec3 p1 = (pMin  - ray.origin()) / ray.direction();
    //     Vec3 p2 = (pMin - ray.origin()) / ray.direction();
    //     Vec3 tMin = Vec3{std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()), std::min(p1.z(), p2.z())};
    //     Vec3 tMax = Vec3{std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y()), std::max(p1.z(), p2.z())};

    //     double max = std::max(tMin.x(), std::max(tMin.y(), tMin.z()));
    //     double min = std::min(tMax.x(), std::min(tMax.y(), tMax.z()));
    //     return max < min;
    // }

    // rayTracing in next week
    bool hit(const Ray& ray, double ray_tmin, double ray_tmax) const {
        for(int i = 0; i < 3; ++i) {
            double invD = 1 / ray.direction()[i];
            double ori = ray.origin()[i];

            double t0 = (pMin[i] - ori) * invD;
            double t1 = (pMax[i] - ori) * invD;

            if(invD < 0) {
                std::swap(t0, t1);
            }

            if(t0 > ray_tmin) { ray_tmin = t0; }
            if(t1 < ray_tmax) { ray_tmax = t1; }

            if(ray_tmax <= ray_tmin) {
                return false;
            }
        }
        return true;
    }
    Bounds pad() {
        // Return an AABB that has no side narrower than some delta, padding if necessary.
        double delta = 0.0001;
        double interval = delta /2;
        Vec3 res1 = pMin;
        Vec3 res2 = pMax;
        for(int  i = 0; i < 3; ++i) {
            if(std::fabs(res1[i] - res2[i]) <= delta) {
                res1[i] -= interval;
                res2[i] += interval;
            }
        }
        return Bounds{res1, res2};
    }

private:
    Vec3 pMin,pMax;
};

inline Bounds operator+(const Bounds& box, const Vec3& offset) {
    return Bounds{box.Min() + offset, box.Max() + offset};
}
inline Bounds operator+(const Vec3& offset, const Bounds& box) {
    return box + offset;
}
}

#endif