#include "Engine/Model/BVH.hpp"
#include "Engine/Model/Material.hpp"
#include "Engine/Model/model.h"
#include "Engine/Render.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Tracer.h"
#include "Engine/camera/camera.h"
#include "Shader/shader.hpp"
#include "maths/Matrix.hpp"
#include "maths/maths.hpp"
#include "pch.h"
#include "texture.hpp"
#include "tools/load.hpp"
#include "ui/window.h"
#include "nlohmann/json.hpp"
#include "util.hpp"
#include <fstream>
#include <memory>
const int width  = 600;
const int height = 600;
const int depth = 255;

using render::Vec3;
using render::Vec2;
using render::Vec4;
using render::Mat3;
using  render::Mat4;
using std::make_shared;
Vec3 light_dir{0 , 0, 1};

void jsonTest() {
    std::ifstream ifs("test.json");
    nlohmann::json data = nlohmann::json::parse(ifs);
    std::cout << data << '\n';
}

void manySphere() {
    render::Tracer tracer;
    render::Scene world;
    auto ground_material = make_shared<render::Lambertian>(Vec3{0.5, 0.5, 0.5});
    world.add(make_shared<render::Sphere>(Vec3{0,-1000,0}, 1000, ground_material));
    world.setBackground(Vec3{0.70, 0.80, 1.00});
    // auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    // world.add(make_shared<render::Sphere>(Vec3{0,-1000,0), 1000, make_shared<render::Lambertian>(checker)));
    // for (int a = -11; a < 11; a++) {
    //     for (int b = -11; b < 11; b++) {
    //         auto choose_mat = random_double();
    //         Vec3 center{a + 0.9*random_double(), 0.2, b + 0.9*random_double()};

    //         if ((center - Vec3{4., 0.2, 0.}).norm() > 0.9) {
    //             std::shared_ptr<render::Material> sphere_material;

    //             if (choose_mat < 0.8) {
    //                 // diffuse
    //                 auto albedo = render::random_Vec3().mul(render::random_Vec3());
    //                 sphere_material = make_shared<render::Lambertian>(albedo);
    //                 // auto center2 = center + Vec3{0., random_double(0, .5), 0.};
    //                 // world.add(make_shared<render::Sphere>(center, center2, 0.2, sphere_material));
    //                 world.add(make_shared<render::Sphere>(center, 0.2, sphere_material));
    //             } else if (choose_mat < 0.95) {
    //                 // render::Metal
    //                 auto albedo = render::random_Vec3(0.5, 1);
    //                 auto fuzz = random_double(0, 0.5);
    //                 sphere_material = make_shared<render::Metal>(albedo, fuzz);
    //                 world.add(make_shared<render::Sphere>(center, 0.2, sphere_material));
    //             } else {
    //                 // glass
    //                 sphere_material = make_shared<render::Dielectric>(1.5);
    //                 world.add(make_shared<render::Sphere>(center, 0.2, sphere_material));
    //             }
    //         }
    //     }
    // }
    auto material1 = make_shared<render::Dielectric>(1.5);
    world.add(make_shared<render::Sphere>(Vec3{0, 1, 0}, 1.0, material1));

    // auto material2 = make_shared<render::Lambertian>(Vec3{0.4, 0.2, 0.1});
    // world.add(make_shared<render::Sphere>(Vec3{-4, 1, 0}, 1.0, material2));

    // auto material3 = make_shared<render::Metal>(Vec3{0.7, 0.6, 0.5}, 0.0);
    // world.add(make_shared<render::Sphere>(Vec3{4, 1, 0}, 1.0, material3));

    render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{13,2,3}, Vec3{0, 1, 0}, Vec3{0, 0,0});
    cam->zoom_ = 10;
    cam->defocus_angle = 0.6;
    tracer.loadCamera(cam);
    render::Editor window;
    window.init();
    window.loadRender(&tracer);
    window.loadScene(&world);
    window.draw();
}

void two_spheres() {
    render::Scene world;
    render::Tracer tracer;
    // auto checker = make_shared<render::CheckerTexture>(0.8, TGAColor{.2, .3, .1), color(.9, .9, .9));
    world.setBackground(Vec3{0.70, 0.80, 1.00});

    auto checker = make_shared<render::CheckerTexture>(0.8, Vec3{.1, .3, .2}, Vec3{.9, .9, .9});

    world.add(make_shared<render::Sphere>(Vec3{0,-10, 0}, 10, make_shared<render::Lambertian>(checker)));
    world.add(make_shared<render::Sphere>(Vec3{0, 10, 0}, 10, make_shared<render::Lambertian>(checker)));

    render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{13,2,3}, Vec3{0, 1, 0}, Vec3{0, 0,0});
    cam->zoom_ = 10;
    cam->defocus_angle = 0;
    tracer.loadCamera(cam);

    // cam.aspect_ratio      = 16.0 / 9.0;
    // cam.image_width       = 400;
    // cam.samples_per_pixel = 100;
    // cam.max_depth         = 50;

    // cam.vfov     = 20;
    // cam.lookfrom = Vec3{13,2,3);
    // cam.lookat   = Vec3{0,0,0);
    // cam.vup      = Vec3{0,1,0);

    // cam.defocus_angle = 0;

    render::Editor window;
    window.init();
    // window.loadRender(&render);
    window.loadRender(&tracer);
    window.loadScene(&world);
    // std::cout << " before draw " << '\n';
    window.draw();
}

void two_perlin_spheres() {
    render::Scene world;
    render::Tracer tracer;
    world.setBackground(Vec3{0.70, 0.80, 1.00});
    auto pertext = std::make_shared<render::NoiseTexture>(4);
    world.add(std::make_shared<render::Sphere>(Vec3{0, -1000, 0}, 1000, std::make_shared<render::Lambertian>(pertext)));
    world.add(std::make_shared<render::Sphere>(Vec3{0, 2, 0}, 2, std::make_shared<render::Lambertian>(pertext)));
    render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{13,2,3}, Vec3{0, 1, 0}, Vec3{0, 0,0});
    cam->zoom_ = 10;
    cam->defocus_angle = 0;
    tracer.loadCamera(cam);

     render::Editor window;
    window.init();
    // window.loadRender(&render);
    window.loadRender(&tracer);
    window.loadScene(&world);
    // std::cout << " before draw " << '\n';
    window.draw();
}


void quads() {
    render::Scene world;
    render::Tracer tracer;
    world.setBackground(Vec3{0.70, 0.80, 1.00});
    // Materials
    auto left_red     = make_shared<render::Lambertian>(Vec3{1.0, 0.2, 0.2});
    auto back_green   = make_shared<render::Lambertian>(Vec3{0.2, 1.0, 0.2});
    auto right_blue   = make_shared<render::Lambertian>(Vec3{0.2, 0.2, 1.0});
    auto upper_orange = make_shared<render::Lambertian>(Vec3{1.0, 0.5, 0.0});
    auto lower_teal   = make_shared<render::Lambertian>(Vec3{0.2, 0.8, 0.8});

    // Quads
    world.add(make_shared<render::Quad>(Vec3{-3,-2, 5}, Vec3{0, 0,-4}, Vec3{0, 4, 0}, left_red));
    world.add(make_shared<render::Quad>(Vec3{-2,-2, 0}, Vec3{4, 0, 0}, Vec3{0, 4, 0}, back_green));
    world.add(make_shared<render::Quad>(Vec3{ 3,-2, 1}, Vec3{0, 0, 4}, Vec3{0, 4, 0}, right_blue));
    world.add(make_shared<render::Quad>(Vec3{-2, 3, 1}, Vec3{4, 0, 0}, Vec3{0, 0, 4}, upper_orange));
    world.add(make_shared<render::Quad>(Vec3{-2,-3, 5}, Vec3{4, 0, 0}, Vec3{0, 0,-4}, lower_teal));
    // world.add(make_shared<render::Sphere>(Vec3{-3, -2, 5}, 2, left_red));

    render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{0,0,9}, Vec3{0, 1, 0}, Vec3{0, 0,0});
    cam->zoom_ = 40;
    cam->defocus_angle = 0;
    tracer.loadCamera(cam);

     render::Editor window;
    window.init();
    // window.loadRender(&render);
    window.loadRender(&tracer);
    window.loadScene(&world);
    // std::cout << " before draw " << '\n';
    window.draw();
}


void simple_light() {
    render::Scene world;
    render::Tracer tracer;
    world.setBackground(Vec3{0.0, 0.0, 0.0});

    auto pertext = make_shared<render::NoiseTexture>(4);
    world.add(make_shared<render::Sphere>(Vec3{0,-1000,0}, 1000, make_shared<render::Lambertian>(pertext)));
    world.add(make_shared<render::Sphere>(Vec3{0,2,0}, 2, make_shared<render::Lambertian>(pertext)));

    auto difflight = make_shared<render::DiffuseLight>(Vec3{4,4,4});
     world.add(make_shared<render::Sphere>(Vec3{0,7,0}, 2, difflight));
    world.add(make_shared<render::Quad>(Vec3{3,1,-2}, Vec3{2,0,0}, Vec3{0,2,0}, difflight));

    
    render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{26,3,6}, Vec3{0, 1, 0}, Vec3{0, 2, 0});
    cam->zoom_ = 10;
    cam->defocus_angle = 0;
    tracer.loadCamera(cam);

     render::Editor window;
    window.init();
    // window.loadRender(&render);
    window.loadRender(&tracer);
    window.loadScene(&world);
    // std::cout << " before draw " << '\n';
    window.draw();
}

void cornell_box() {
    render::Scene world;
    render::Tracer tracer;
    world.setBackground(Vec3{0.0, 0.0, 0.0});

    auto red   = make_shared<render::Lambertian>(Vec3{.65, .05, .05});
    auto white = make_shared<render::Lambertian>(Vec3{.73, .73, .73});
    auto green = make_shared<render::Lambertian>(Vec3{.12, .45, .15});
    // auto light = make_shared<render::DiffuseLight>(Vec3{15, 15, 15});
    auto light = make_shared<render::DiffuseLight>(Vec3{7, 7, 7});

    world.add(make_shared<render::Quad>(Vec3{555,0,0}, Vec3{0,555,0}, Vec3{0,0,555}, green));
    world.add(make_shared<render::Quad>(Vec3{0,0,0}, Vec3{0,555,0}, Vec3{0,0,555}, red));
    world.add(make_shared<render::Quad>(Vec3{343, 554, 332}, Vec3{-130,0,0}, Vec3{0,0,-105}, light));
    world.add(make_shared<render::Quad>(Vec3{0,0,0}, Vec3{555,0,0}, Vec3{0,0,555}, white));
    world.add(make_shared<render::Quad>(Vec3{555,555,555}, Vec3{-555,0,0}, Vec3{0,0,-555}, white));
    world.add(make_shared<render::Quad>(Vec3{0,0,555}, Vec3{555,0,0}, Vec3{0,555,0}, white));
    // world.add(make_shared<render::Sphere>(Vec3{250,250, 250}, 100, white));
    // world.add(box(Vec3{130, 0, 65}, Vec3{295, 165, 230}, white));
    // world.add(box(Vec3{265, 0, 295}, Vec3{430, 330, 460}, white));

    
    std::shared_ptr<render::Model> box1 = box(Vec3{0,0,0}, Vec3{165,330,165}, white);
    box1 = make_shared<render::YRotate>(box1, 15);
    box1 = make_shared<render::Translate>(box1, Vec3{265,0,295});
    // box1 = make_shared<render::YRotate>(box1, 15);
    world.add(box1);

    std::shared_ptr<render::Model> box2 = box(Vec3{0,0,0}, Vec3{165,165,165}, white);
    // std::shared_ptr<render::Model> box2 = box(Vec3{130,0,65}, Vec3{165,165,165}, white);
    box2 = make_shared<render::YRotate>(box2, -18);
    box2 = make_shared<render::Translate>(box2, Vec3{130,0,65});
    // box2 = make_shared<render::YRotate>(box2, -18);
    world.add(box2);

    render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{278, 278, -800}, Vec3{0, 1, 0}, Vec3{278, 278, 0});
    cam->zoom_ = 20;
    cam->defocus_angle = 0;
    tracer.loadCamera(cam);

    render::Editor window;
    window.init();
    // window.loadRender(&render);
    window.loadRender(&tracer);
    window.loadScene(&world);
    // std::cout << " before draw " << '\n';
    window.draw();
}


void cornell_smoke() {
    render::Scene world;
    render::Tracer tracer;
    world.setBackground(Vec3{0.0, 0.0, 0.0});

    auto red   = make_shared<render::Lambertian>(Vec3{.65, .05, .05});
    auto white = make_shared<render::Lambertian>(Vec3{.73, .73, .73});
    auto green = make_shared<render::Lambertian>(Vec3{.12, .45, .15});
    auto light = make_shared<render::DiffuseLight>(Vec3{7, 7, 7});

    world.add(make_shared<render::Quad>(Vec3{555,0,0}, Vec3{0,555,0}, Vec3{0,0,555}, green));
    world.add(make_shared<render::Quad>(Vec3{0,0,0}, Vec3{0,555,0}, Vec3{0,0,555}, red));
    world.add(make_shared<render::Quad>(Vec3{113,554,127}, Vec3{330,0,0}, Vec3{0,0,305}, light));
    world.add(make_shared<render::Quad>(Vec3{0,555,0}, Vec3{555,0,0}, Vec3{0,0,555}, white));
    world.add(make_shared<render::Quad>(Vec3{0,0,0}, Vec3{555,0,0}, Vec3{0,0,555}, white));
    world.add(make_shared<render::Quad>(Vec3{0,0,555}, Vec3{555,0,0}, Vec3{0,555,0}, white));

    std::shared_ptr<render::Model> box1 = box(Vec3{0,0,0}, Vec3{165,330,165}, white);
    box1 = make_shared<render::YRotate>(box1, 15);
    box1 = make_shared<render::Translate>(box1, Vec3{265,0,295});
    // world.add(box1);

    std::shared_ptr<render::Model> box2 = box(Vec3{0,0,0}, Vec3{165,165,165}, white);
    box2 = make_shared<render::YRotate>(box2, -18);
    box2 = make_shared<render::Translate>(box2, Vec3{130,0,65});
    // world.add(box2);

    world.add(make_shared<render::ConstantMedium>(box1, 0.01, Vec3{0,0,0}));
    world.add(make_shared<render::ConstantMedium>(box2, 0.01, Vec3{1,1,1}));

     render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{278, 278, -800}, Vec3{0, 1, 0}, Vec3{278, 278, 0});
    cam->zoom_ = 20;
    cam->defocus_angle = 0;
    tracer.loadCamera(cam);

    render::Editor window;
    window.init();
    // window.loadRender(&render);
    window.loadRender(&tracer);
    window.loadScene(&world);
    // std::cout << " before draw " << '\n';
    window.draw();
}


void final_scene() {
    render::Scene world;
    render::Tracer tracer;
    render::Objects boxes1;
    world.setBackground(Vec3{0.0, 0.0, 0.0});
    auto ground = make_shared<render::Lambertian>(Vec3{.48, 0.83, 0.53});
    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 100);
            auto z1 = z0 + w;
            boxes1.add(box(Vec3{x0,y0,z0}, Vec3{x1,y1,z1}, ground));
        }
    }
    world.add(make_shared<render::BVH>(boxes1));
    auto light = make_shared<render::DiffuseLight>(Vec3{7, 7, 7});
    world.add(make_shared<render::Quad>(Vec3{123,554,147}, Vec3{300,0,0}, Vec3{0,0,265}, light));

    auto center1 = Vec3{400, 400, 200};
    auto center2 = center1 + Vec3{30,0,0};
    auto sphere_material = make_shared<render::Lambertian>(Vec3{0.7, 0.3, 0.1});
    world.add(make_shared<render::Sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<render::Sphere>(Vec3{260, 150, 45}, 50, make_shared<render::Dielectric>(1.5)));
    world.add(make_shared<render::Sphere>(
        Vec3{0, 150, 145}, 50, make_shared<render::Metal>(Vec3{0.8, 0.8, 0.9}, 1.0)
    ));

    auto boundary = make_shared<render::Sphere>(Vec3{360,150,145}, 70, make_shared<render::Dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<render::ConstantMedium>(boundary, 0.2, Vec3{0.2, 0.4, 0.9}));
    boundary = make_shared<render::Sphere>(Vec3{0,0,0}, 5000, make_shared<render::Dielectric>(1.5));
    world.add(make_shared<render::ConstantMedium>(boundary, .0001, Vec3{1,1,1}));

    auto emat = make_shared<render::Lambertian>(make_shared<render::ImageTexture>("H:\\code\\render\\assets\\images\\earthmap.jpg"));
    world.add(make_shared<render::Sphere>(Vec3{400,200,400}, 100, emat));
    auto pertext = make_shared<render::NoiseTexture>(0.1);
    world.add(make_shared<render::Sphere>(Vec3{220,280,300}, 80, make_shared<render::Lambertian>(pertext)));

    render::Objects boxes2;
    auto white = make_shared<render::Lambertian>(Vec3{.73, .73, .73});
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<render::Sphere>(render::random_Vec3(0,165), 10, white));
    }

    world.add(make_shared<render::Translate>(
        make_shared<render::YRotate>(
            make_shared<render::BVH>(boxes2), 15),
            Vec3{-100,270,395}
        )
    );

    
    render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{478, 278, -600}, Vec3{0, 1, 0}, Vec3{278, 278, 0});
    // render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{500, 1000, 500}, Vec3{1, 0, 0}, Vec3{0, 0, 0});
    cam->zoom_ = 20;
    cam->defocus_angle = 0;
    tracer.loadCamera(cam);

    render::Editor window;
    window.init();
    // window.loadRender(&render);
    window.loadRender(&tracer);
    window.loadScene(&world);
    // std::cout << " before draw " << '\n';
    window.draw();
}

void test_box() {
    render::Scene world_;
    render::Tracer tracer;
    render::Objects world;
    world_.setBackground(Vec3{0.0, 0.0, 0.0});

    auto red   = make_shared<render::Lambertian>(Vec3{.65, .05, .05});
    auto white = make_shared<render::Lambertian>(Vec3{.73, .73, .73});
    auto green = make_shared<render::Lambertian>(Vec3{.12, .45, .15});
    auto light = make_shared<render::DiffuseLight>(Vec3{15, 15, 15});

    world.add(make_shared<render::Quad>(Vec3{555,0,0}, Vec3{0,555,0}, Vec3{0,0,555}, green));
    world.add(make_shared<render::Quad>(Vec3{0,0,0}, Vec3{0,555,0}, Vec3{0,0,555}, red));
    world.add(make_shared<render::Quad>(Vec3{343, 554, 332}, Vec3{-130,0,0}, Vec3{0,0,-105}, light));
    world.add(make_shared<render::Quad>(Vec3{0,0,0}, Vec3{555,0,0}, Vec3{0,0,555}, white));
    world.add(make_shared<render::Quad>(Vec3{555,555,555}, Vec3{-555,0,0}, Vec3{0,0,-555}, white));
    world.add(make_shared<render::Quad>(Vec3{0,0,555}, Vec3{555,0,0}, Vec3{0,555,0}, white));
    // world.add(box(Vec3{130, 0, 65}, Vec3{295, 165, 230}, white));
    // world.add(box(Vec3{265, 0, 295}, Vec3{430, 330, 460}, white));
    auto bvh = std::make_shared<render::BVH>(world);
    // std::cout << " =================\n";
    // std::cout << bvh->bbBox().Min() << '\n';
    // std::cout << bvh->bbBox().Max() << '\n';
    world_.add(bvh);
    // std::shared_ptr<render::Model> box1 = box(Vec3{0,0,0}, Vec3{165,330,165}, white);
    // box1 = make_shared<render::YRotate>(box1, 15);
    // box1 = make_shared<render::Translate>(box1, Vec3{265,0,295});
    // world.add(box1);

    // std::shared_ptr<render::Model> box2 = box(Vec3{0,0,0}, Vec3{165,165,165}, white);
    // // box2 = make_shared<render::YRotate>(box2, -18);
    // // box2 = make_shared<render::Translate>(box2, Vec3{130,0,65});
    // world.add(box2);

    render::Camera* cam = render::Camera::GetCamera(render::Camera::NONE, Vec3{278, 278, -800}, Vec3{0, 1, 0}, Vec3{278, 278, 0});
    cam->zoom_ = 20;
    cam->defocus_angle = 0;
    tracer.loadCamera(cam);

    render::Editor window;
    window.init();
    // window.loadRender(&render);
    window.loadRender(&tracer);
    window.loadScene(&world_);
    // std::cout << " before draw " << '\n';
    window.draw();
}


int main() {
    // render::Render render;
    // render::Node node[3];
    // node[0].coords = Vec4{1., 0.0, 0.2, 1.};
    // node[1].coords = Vec4{0., 1., 0.5, 1.};
    // node[2].coords = Vec4{0., 0., 1., 1.};
    // node[0].diffuse = render::TGAColor{255, 0, 0};
    // node[1].diffuse = render::TGAColor{0, 255, 0};
    // node[2].diffuse = render::TGAColor{0, 0, 255};
    // render.loadModel(node);
    // render.loadModel(load());
    // render::Node node;
    // node.coords = Vec4{0, 0, 1, 1};
    // render.loadModel(node, 0.3);
    // render.loadShader(new render::CircleShader());
    // render.loadShader(new render::GouraudShader());
    // render::Tracer tracer;
    // render::Scene world;
    // auto material_ground = std::make_shared<render::Lambertian>(Vec3{0.8, 0.8, 0.0});
    // auto material_center = std::make_shared<render::Lambertian>(Vec3{0.1, 0.2, 0.5});
    // // auto material_left   = std::make_shared<render::Metal>(Vec3{0.8, 0.8, 0.8}, 0.3);
    // auto material_right  = std::make_shared<render::Metal>(Vec3{0.8, 0.6, 0.2}, 0.0);
    // // auto material_center = make_shared<render::Dielectric>(1.5);
    // auto material_left   = make_shared<render::Dielectric>(1.5);

    // world.add(make_shared<render::Sphere>(Vec3{0.0, -100.5, -1.0}, 100.0, material_ground));
    // world.add(make_shared<render::Sphere>(Vec3{0.0,    0.0, -1.0},   0.5, material_center));
    // world.add(make_shared<render::Sphere>(Vec3{-1.0,    0.0, -1.0},   0.5, material_left));
    // world.add(make_shared<render::Sphere>(Vec3{-1.0, 0.0 -1.0}, -0.4, material_left));
    // world.add(make_shared<render::Sphere>(Vec3{1.0,    0.0, -1.0},   0.5, material_right));
    // // render::Window window;
    // render::Editor window;
    // window.init();
    // // window.loadRender(&render);
    // window.loadRender(&tracer);
    // window.loadScene(&world);
    // // std::cout << " before draw " << '\n';
    // window.draw();
    // render::Editor window;
    // render.render();

    // render::Model model(load());
    // std::cout << model.model() << '\n';
    // render.loadJson(load());
    // manySphere();
    // two_perlin_spheres();
    // quads();
    // simple_light();
    // cornell_box();
    final_scene();
    // two_spheres();
    // cornell_smoke();
    // test_box();


}