#include "Scene.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Model/Model.h"
#include "maths/maths.hpp"
#include "texture.hpp"
#include "tgaimage.h"
#include <limits>
#include <numbers>

namespace render {
// BRDF

// 计算法线分布函数D Trowbridge-Reitz GGX
// N为法线，H为半程向量, a为粗糙度
double DistributionGG(Vec3 N, Vec3 H, double roughness) {
    double a = roughness * roughness;
    double a2     = a * a;
    double NdotH  = std::max(N * H, 0.0);
    double NdotH2 = NdotH*NdotH;

    double nom    = a2;
    double denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = std::numbers::pi * denom * denom;
    return nom / denom;
}

// 几何函数 Schlick-GGX
double GeometrySchlickGGX(double NdotV, double roughness) {
    double k = roughness * roughness / 2.;       // IBL 
    // double k = (roughness + 1) * (roughness + 1) / 8;  //direct 直接光照
    double nom   = NdotV;
    double denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// N 法线 V 视线 L 光照方向
double GeometrySmith(Vec3 N, Vec3 V, Vec3 L, double roughness) {
    double NdotV = std::max(N * V, 0.0);
    double NdotL = std::max(N * L, 0.0);
    double ggx1 = GeometrySchlickGGX(NdotV, roughness);
    double ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// 菲涅尔方程
// vec3 F0 = vec3(0.04);
// F0      = mix(F0, albedo, metalness);
double fresnelSchlick(double cosTheta, double F0) {
    return F0 + (1 - F0) * pow(std::max(std::min(0., 1.0 - cosTheta), 1.0), 5.0);
}

double fresnelSchlick_IBL(double cosTheta, double F0, double roughness) {
    return F0 + (1.0 - F0) * pow(std::max(std::min(0., 1.0 - cosTheta), 1.0), 5.0);
}

// vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);
// float NDF = DistributionGGX(N, H, roughness);       
// float G   = GeometrySmith(N, V, L, roughness);    
// vec3 nominator    = NDF * G * F;
// float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
// vec3 specular     = nominator / denominator;  
// vec3 kS = F;
// vec3 kD = vec3(1.0) - kS;

// kD *= 1.0 - metallic;   


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
    double res = hitSphere(Vec3{0.5, 0.5, -1.0f}, 10, r);
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


Vec3 random_on_hemisphere(const Vec3& normal) {
    Vec3 vec = random_unit_vector();
    return normal * vec >= 0.0 ? vec : -1 * vec;
}



Vec3 Scene::castRay(const Ray &ray, int depth) const {
    if(depth >= maxDepth) {
        return Vec3{0.0,0.0, 0.0};
    }
    Intersection rec;
    bool flag = false;
    Intersection pos;
    double tFar = std::numeric_limits<double>::infinity();
    double ray_tmin = 0.001; // 避免浮点误差，导致从物体表面射出的光线与物体相交
    for(const auto& model : models_) {
        if(model->hit(ray, ray_tmin, tFar, pos)) {
            flag = true;
            tFar = pos.t; 
            rec = pos;
        }
    }
    // 如果光线没有击中任何物体
    if(!flag) {
        return background_;
    }
    Ray scatted;
    Vec3 albedo;
    Vec3 emitColor = rec.mat->emitted(rec.uv, rec.p);
    if(!pos.mat->scatter(ray, pos, albedo, scatted)) {
        return emitColor;
    }
    // 自发光项 + 反射/折射
    return emitColor + albedo.mul(castRay(scatted, depth + 1));
}
void Scene::sampleLight(Intersection& pos) const {
    return ;
}
}