#include "Tracer.h"
#include "Engine/Model/model.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Tracer.h"
#include "Engine/camera/camera.h"
#include "tgaimage.h"
#include "util.hpp"
#include "GLFW/glfw3.h"
#include <gl/GL.h>
#include <limits>
#include <stdlib.h>
#include <time.h>
#include "omp.h"
namespace render {
// static Camera* camera = Camera::GetCamera(Camera::NONE, Vec3{-2, 2, -1}, Vec3{0, 1, 0}, Vec3{0, 0, -1});
const double infinity = std::numeric_limits<double>::infinity();
double hit_Sphere(const Vec3& center, double radius, const Ray& ray) {
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

TGAColor  ray_color(const Ray& r, const Model& model) {
    Intersection rec;
    if(model.hit(r, 0, infinity, rec)) {
        Vec3 normal = rec.normal;
        for(int i = 0; i < 3; ++i) {
            normal[i] = 0.5 * (normal[i] + 1);
        }
        return TGAColor::fromVec3(normal);
    }
    Vec3 direction = r.direction().normalized();
    double a = 0.5 * (direction.y() + 1.0);
    return TGAColor{255, 255, 255} * (1.0 - a) + TGAColor{255, (uint8_t)(0.7 * 255), (uint8_t)(0.5 * 255)} * a;
}
Vec3 pixel_sample_square(Vec3 u, Vec3 v)  {
    // Returns a random point in the square surrounding a pixel at the origin.
    auto px = -0.5 + random_double();
    auto py = -0.5 + random_double();
    return px * u + py * v;
}

void linear_to_gamma(Vec3& color) {
    for(int i = 0; i < 3; ++i) {
        color[i] = std::sqrt(color[i]);
    }
}

void Tracer::render(const Scene& scene) {
    Vec3 camera_center = camera_->position_;
    auto [viewport_width, viewport_height] = camera_->ViewportLength();
    std::cout << viewport_width << " | " << viewport_height << '\n';
    Vec3 w = (camera_->position_ - camera_->target_).normalized();
    Vec3 u = camera_->worldup_.cross(w).normalized();
    Vec3 v = w.cross(u);
    Vec3 viewport_delta_u = viewport_width * u / width;
    Vec3 viewport_delta_v = -viewport_height / height * v;

    Vec3 up_left = camera_center - camera_->focus_dist * w - viewport_width * u/2 +viewport_height * v/2;
    Vec3 pixel00_loc = up_left + 0.5 * (viewport_delta_u + viewport_delta_v);
    auto defocus_radius = camera_->focus_dist * std::tan(deg2rad(camera_->defocus_angle / 2));
    Vec3 defocus_disk_u = u * defocus_radius;
    Vec3 defocus_disk_v = v * defocus_radius;
    int  spp = 5000;
    std::cout << viewport_width << " " << viewport_height << '\n';
    std::cout << viewport_delta_u << " " << viewport_delta_v << '\n';
    clock_t start = clock();
// #pragma omp parallel for 
    for(int i = 0; i < width; ++i) {
        // int rank = omp_get_thread_num();
        // if(rank == 0) {
        clock_t now = clock();
        double time = (double)(now - start)/ CLOCKS_PER_SEC;
        double speed = i > 0 ? i / time : 1;
        double sec = (width - i) / speed;
        std::clog << "\rScanlines remaining: " << (int)((1.0 - 1.0 *(width - i) / width) * 100) << "%  " 
            << "need " << sec << "s" << std::flush;
        // }
        for(int j = 0; j < height; ++j) {
            Vec3 color{};

            Vec3 center = pixel00_loc + 1.0 * i  * viewport_delta_u + 1.0 * j  *viewport_delta_v;
            for(int k = 0; k < spp; ++k) {
                // Get a randomly-sampled camera_ ray for the pixel at location i,j, originating from
                // the camera_ defocus disk.
                Vec3 origin = camera_->defocus_angle <= 0 ? 
                    camera_center : [&]() { 
                    auto p = random_in_unit_disk();
                    return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
                }();
                Vec3 direction = center + pixel_sample_square(viewport_delta_u, viewport_delta_v) - origin;
                double time = random_double();
                Ray ray(origin, direction, time);
                Vec3 tmp = scene.castRay(ray, 0);
                color += tmp;
            }
            color = color *  (1.0/ spp);
            linear_to_gamma(color); // gamma校正
            color.clamp(0., 0.999);
            image_->set(i, j, TGAColor::fromVec3(color));
        }
    }
    // image_->flip_horizontally();
    image_->flip_vertically();
    std::clog << "\rDone.                 \n";
}

void Tracer::clear() {
}

void Tracer::loadBuffer(TGAImage* image) {
    image_ = image;
}
void Tracer::loadCamera(Camera * camera) {
    camera_ = camera;
}
}