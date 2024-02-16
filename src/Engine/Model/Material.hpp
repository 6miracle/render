#ifndef __RENDER_MATERIAL_HPP__
#define __RENDER_MATERIAL_HPP__

#include "Engine/Model/Intersection.hpp"
#include "Engine/Model/Light.hpp"
#include "Engine/Model/Ray.hpp"
#include "maths/maths.hpp"
#include "texture.hpp"
#include "tgaimage.h"
#include "util.hpp"
#include <memory>
namespace render {

// RayTracing in one week 没有采用设置大量参数的做法，而是封装特定方法
// 方法求出出射光线和反射率
class Material {
public:

    virtual ~Material() = default;
    virtual bool scatter(const Ray& in, const Intersection& pos, Vec3& albedo, Ray& scattered) const = 0;
    virtual Vec3 emitted(Vec2 uv, const Vec3& p) const { return Vec3{0, 0, 0}; }
private:

};


class Lambertian: public Material {
public:
    Lambertian(const Vec3& albedo): albedo_(std::make_shared<SolidColor>(albedo)) {}
    Lambertian(std::shared_ptr<Texture> a): albedo_(a) {}
    bool scatter(const Ray& in, const Intersection& pos, Vec3& albedo, Ray& scattered) const override {
        Vec3 direction = pos.normal + random_unit_vector();
        if(direction.near_zero()) { direction = pos.normal; }
        scattered = Ray{pos.p, direction, in.time()};  // 完善运动模糊，加入时间
        albedo = albedo_->value(pos.uv, pos.p);
        return true;
    }
private:    
    // Vec3 albedo_;
    std::shared_ptr<Texture> albedo_;
};

// 金属 会反射光线
class Metal: public Material {
public:
    Metal(const Vec3& albedo, double f): albedo_(albedo), fuzz_(f) {}
     bool scatter(const Ray& in, const Intersection& pos, Vec3& albedo, Ray& scattered) const override {
        Vec3 direction = reflect(in.direction(), pos.normal);
        scattered = Ray{pos.p, direction + fuzz_ * random_unit_vector()}; // 模糊反射
        albedo = albedo_;
        return true;
    }

private:
    Vec3 albedo_; // 反射率
    double fuzz_; // 模糊反射，金属度？
};


// 折射
class Dielectric: public Material {
public:
    Dielectric(double ir): ir_(ir) {}
    bool scatter(const Ray& in, const Intersection& pos, Vec3& albedo, Ray& scattered) const {
        albedo = Vec3{1.0, 1.0, 1.0};
        double refraction_ratio = pos.front ? (1.0 / ir_) : ir_;
        Vec3 direction;

        Vec3 vec= in.direction().normalized();
        double cos = std::min((-1 * vec) * pos.normal, 1.0);
        double sin = std::sqrt(1 - cos *cos);

        // 折射角度是否符合要求，以及判断菲涅尔项
        if((refraction_ratio * sin > 1.0) || reflectance(cos, refraction_ratio) > random_double()) {   // 
            // 必须反射
            direction = reflect(vec, pos.normal);
        } else {
            // 折射
            direction = refract(vec, pos.normal, refraction_ratio);
        }
        scattered = Ray{pos.p, direction, in.time()};
        return true;
    }

    // 菲涅尔项的Schlick近似
    static double reflectance(double cosTheta, double rate) {
        double F0 = (1.0 - rate) / (1.0 + rate);
        F0 *= F0;
        // return F0  + (1 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
        return F0  + (1 - F0) * pow(1.0 - cosTheta, 5.0);
    }

private:
    double ir_;  // index of refraction, 折射率
};


// 自发光材质
class DiffuseLight: public Material {
public:
    DiffuseLight(std::shared_ptr<Texture> emit): emit_(emit) {}
    DiffuseLight(Vec3 color): emit_(std::make_shared<SolidColor>(color)){}

    bool scatter(const Ray& in, const Intersection& pos, Vec3& albedo, Ray& scattered) const override {
        return false;
    }

    Vec3 emitted(Vec2 uv, const Vec3& p) const override {
        return emit_->value(uv, p);
    }

private:
    std::shared_ptr<Texture> emit_;
};

// 各向同性材质
class Isotropic: public Material {
public:
    Isotropic(std::shared_ptr<Texture> tex): tex_(tex) { }
    Isotropic(Vec3 color): tex_(std::make_shared<SolidColor>(color)) {}
    bool scatter(const Ray& in, const Intersection& pos, Vec3& albedo, Ray& scattered) const override {
        scattered = Ray{pos.p, random_unit_vector(), in.time()};
        albedo = tex_->value(pos.uv, pos.p);
        return true;
    }
private:
    std::shared_ptr<Texture> tex_;

};
}

#endif