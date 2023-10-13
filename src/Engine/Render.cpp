#include "Render.h"
#include "Engine/Model/model.h"
#include "Engine/Render.h"
#include "Engine/Scene/Scene.h"
#include "Engine/camera/camera.h"
#include "maths/Matrix.hpp"
#include "maths/maths.hpp"
#include "tgaimage.h"
#include "util.hpp"
#include <array>
#include <cmath>
#include "GLFW/glfw3.h"
#include <gl/GL.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include "nlohmann/json.hpp"

namespace render {

enum PLANE {W_PLANE, X_LEFT, X_RIGHT, Y_UP, Y_DOWN, Z_NEAR, Z_FAR};
static Camera* camera = Camera::GetCamera();
Mat4 modelMatrix(double angle);
Mat4 pespectiveMatrix(double eye_fov, double aspect_ratio, double zNear, double zFar);
Mat4 viewPortMatrix(double widthm, double height);
// 求出重心坐标
Vec3 barycentric(Node* nodes, Vec3 p);
// 齐次空间裁剪
int clip(std::vector<Node>& result);
// bounding box
std::array<int, 4> boundBox(Node* nodes);
bool inCycle(Vec3 vec, Vec3 center, double radius);

// IShader
void IShader::setUniform(const std::string& name, const Mat4& mat) {
    map_[name] = mat;
}

// Render
Render::Render(): image_(width, height, 3) { zbuffer_.resize((width + 100) * (height + 100), 0); }

void Render::loadShader(IShader* shader) {
    shader_.reset(shader);
}

void Render::loadModel(const std::string& path) {
    model_.push_back(new ObjModel(path));
}

void Render::loadModel(Node* node) {
    model_.push_back(new TriModel(node));
}

void Render::loadModel(Node& node, double radius) {
    model_.push_back(new BallModel(node, radius));
}

void Render::triangle(Node* nodes) {
    // Vec4 screen_coords[3];
    for (int j = 0; j < 3; j++) { 
        nodes[j].screen_coords = viewPortMatrix(width, height) * (nodes[j].coords /  nodes[j].coords[3]);
    }
    // 寻找bounding box

    // int bottom = std::floor(std::min(std::min(nodes[0].screen_coords.y(), nodes[1].screen_coords.y()), nodes[2].screen_coords.y()));
    // int up = std::ceil(std::max(std::max(nodes[0].screen_coords.y(), nodes[1].screen_coords.y()), nodes[2].screen_coords.y()));
    // int left = std::floor(std::min(std::min(nodes[0].screen_coords.x(), nodes[1].screen_coords.x()), nodes[2].screen_coords.x()));
    // int right = std::ceil(std::max(std::max(nodes[0].screen_coords.x(), nodes[1].screen_coords.x()), nodes[2].screen_coords.x()));
    std::array<int, 4> bbox = boundBox(nodes);
    glBegin(GL_POINTS);
    for(int i = std::max(0, bbox[2]); i <= std::min( width, bbox[3]); ++i) {
        for(int j = std::max(0, bbox[0]); j <= std::min(height, bbox[1]); ++j) {
            Vec3 vec = barycentric(nodes, {i, j, 0});
            if(vec.x() < 0 || vec.y() < 0 || vec.z() < 0) {
                continue;
            }
            vec[0] /= nodes[0].coords.z();
            vec[1] /= nodes[1].coords.z();
            vec[2] /= nodes[2].coords.z();
            // 透视矫正 https://zhuanlan.zhihu.com/p/144331875
            double z = 1.0 / (vec[0] + vec[1] + vec[2]);
            double frag_depth = z;
            if(zbuffer_[i + j * width] == 0 || frag_depth < zbuffer_[i + j * width]) {
                zbuffer_[i + j * width] = frag_depth;
                TGAColor color;
                bool flag = shader_->fragment(nodes, vec * z, color);
                // LOG_ERROR("3");
                if(!flag) {
                    image_.set(i, j, color);
                    // TODO
                    glColor3f(1.0f * color[2] / 255.0f, 1.0 * color[1] / 255.0f, 1.0 * color[0] / 255.0f);
                    glVertex3f( 1.0 * i / width * 2.0 - 1. , 1.0 * j / height * 2.0 - 1., 0.0);
                }
            }
        }
    }
    glEnd();
    // printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`\n");
}
void Render::render() {;
    // view矩阵
    Mat4 view_matrix = camera->getViewMatrix();
    // 投影矩阵
    Mat4 projection_matrix = pespectiveMatrix(camera->zoom_, 1.0 * width / height, 0.1, 100.0);
    shader_->setUniform("view", view_matrix);
    shader_->setUniform("projection", projection_matrix);
    
    for(auto model : model_) {
        // 设置模型
        shader_->setModel(model);
        shader_->setUniform("model", model->model());
        // if(model->type() != Model::Type::Circle) {
            // 设置矩阵
            for (int i = 0; i < model->nfaces(); i++) { 
                Node nodes[3];
                // nodes.resize(3);
                for (int j = 0; j < 3; j++) { 
                    shader_->vertex(nodes[j], i, j);
                } 
                // 齐次空间裁剪(Homogeneous Space Clipping)
                // std::vector<Node> list;
                // list.emplace_back(nodes[0]);
                // list.emplace_back(nodes[1]);
                // list.emplace_back(nodes[2]);
                // int num = clip(list);
                // for(int i = 0; i < num; ++i) {
                //     nodes[0] = list[i];
                //     nodes[1] = list[(i + 1) % num];
                //     nodes[2] = list[(i + 2) % num];
                triangle(nodes); 
                if(model->type() == Model::Type::Ball) {
                    for(int j = 3; j < 6; ++j) {
                        shader_->vertex(nodes[j - 3], i, j);
                    }
                    triangle(nodes);
                }
                // }
            }
        // } else {
        //     // printf("----------\n");
        //     Node node;
        //     shader_->vertex(node, 0, 0);
        //     cycle(node,((CircleModel*)model)->getRadius());
        // }
    }

}

void Render::cycle(Node& node, double radius) {
    for (int j = 0; j < 3; j++) { 
        node.screen_coords = viewPortMatrix(width, height) * (node.coords /  node.coords[3]);
    }
    // std::array<int, 4> bbox = boundBox(nodes);
    int left = node.screen_coords.x() - radius;
    int right = node.screen_coords.x() + radius;
    int up = node.screen_coords.y() + radius;
    int down = node.screen_coords.y() - radius;
    glBegin(GL_POINTS);
    for(int i = std::max(0, left); i <= std::min( width, right); ++i) {
        for(int j = std::max(0, down); j <= std::min(height, up); ++j) {
            Vec3 tmp{i, j, 0};
            if(!inCycle(tmp, homo2local(node.screen_coords), radius)) continue;
            // printf("---------\n");
            // Vec3 vec = barycentric(nodes, tmp);
            // if(vec.x() < 0 || vec.y() < 0 || vec.z() < 0) {
            //     continue;
            // }
            // vec[0] /= nodes[0].coords.z();
            // vec[1] /= nodes[1].coords.z();
            // vec[2] /= nodes[2].coords.z();
            // 透视矫正 https://zhuanlan.zhihu.com/p/144331875
            // double z = 1.0 / (vec[0] + vec[1] + vec[2]);
            // double frag_depth = z;
            // if(zbuffer_[i + j * width] == 0 || frag_depth < zbuffer_[i + j * width]) {
            //     zbuffer_[i + j * width] = frag_depth;
                TGAColor color;
                bool flag = shader_->fragment(&node, tmp, color);
                if(!flag) {
                    image_.set(i, j, color);
                    // TODO
                    glColor3f(1.0f * color[2] / 255.0f, 1.0 * color[1] / 255.0f, 1.0 * color[0] / 255.0f);
                    glVertex3f( 1.0 * i / width * 2.0 - 1. , 1.0 * j / height * 2.0 - 1., 0.0);
                }
            // }
        }
    }
    glEnd();
}
bool insidePlane(PLANE plane, Vec4 vec) {
    switch(plane) {
        case W_PLANE:
            // return true;
            return vec.w() >= 0.0000001;
        case X_LEFT:
            return vec.x() <= vec.w();
        case X_RIGHT:
            return vec.x() >= -vec.w();
        case Y_UP:
            return vec.y() <= vec.w();
        case Y_DOWN:
            return vec.y() >= -vec.w();
        case Z_NEAR:
            return vec.z() <= vec.w();
        case Z_FAR:
            return vec.z() >= -vec.w();
        default:
            ASSERT(false, "plane error");
    }
    return false;
}

double calRatio(Vec4 prev, Vec4 curv, PLANE plane) {
    switch(plane) {
        case W_PLANE:
            return (prev.w() + 0.00001) / (prev.w() - curv.w());
        case X_RIGHT:
            return (prev.w() - prev.x()) / ((prev.w() - prev.x()) - (curv.w() - curv.x()));
        case X_LEFT:
            return (prev.w() + prev.x()) / ((prev.w() + prev.x()) - (curv.w() + curv.x()));
        case Y_UP:
            return (prev.w() - prev.y()) / ((prev.w() - prev.y()) - (curv.w() - curv.y()));
        case Y_DOWN:
            return (prev.w() + prev.y()) / ((prev.w() + prev.y()) - (curv.w() + curv.y()));
        case Z_NEAR:
            return (prev.w() - prev.z()) / ((prev.w() - prev.z()) - (curv.w() - curv.z()));
        case Z_FAR:
            return (prev.w() + prev.z()) / ((prev.w() + prev.z()) - (curv.w() + curv.z()));
        default:
            return 0;
    }
}
int clip_with_plane(PLANE plane, std::vector<Node>& result, int count) {
    std::vector<Node> tmp(std::move(result));
    result.clear();
    int num = 0;
    for(int i = 0; i < count; ++i) {
        Node& cur = tmp[i];
        Node& pre = tmp[(i - 1 + count) % count];

        bool curF = insidePlane(plane, cur.coords);
        bool preF = insidePlane(plane, pre.coords);

        if(curF != preF) {
            double ratio = calRatio(pre.coords, cur.coords, plane);
            Node node;
            node.coords = pre.coords + (cur.coords - pre.coords) * ratio;
            node.uv = pre.uv + (cur.uv - pre.uv) * ratio;
            node.diffuse = pre.diffuse * (1 - ratio) + cur.diffuse * ratio;
            node.normal = pre.normal + (cur.normal - pre.normal) * ratio;
            node.specular = pre.specular + (cur.specular - cur.specular) * ratio;
            node.normalTan = pre.normalTan * (1 - ratio) +  ratio * cur.normalTan;
            result.emplace_back(node);
            ++num;
        } 
        if(curF) {
            result.emplace_back(cur);
            ++num;
        }
     }
     return num;
}
int clip(std::vector<Node>& result) {
    int num = clip_with_plane(W_PLANE, result, 3);
    num = clip_with_plane(Z_NEAR, result, num);
    num = clip_with_plane(Z_FAR, result, num);
    // num = clip_with_plane(X_LEFT, result, num);
    // num = clip_with_plane(X_RIGHT, result, num);
    // num = clip_with_plane(Y_UP, result, num);
    // num = clip_with_plane(Y_DOWN, result, num);
    return num;
}
// 采用unity shader入门精要里的，因为观察空间为右手系，到投影矩阵变换到左手系
Mat4 pespectiveMatrix(double eye_fov, double aspect_ratio, double zNear, double zFar) {
    double angle = eye_fov / 2 * std::numbers::pi / 180;
    Mat4 mat;
    double h = std::tan(angle) * zNear;
    double l =  h * aspect_ratio;
    mat <<  zNear / l, 0, 0, 0,
        0,  zNear / h, 0, 0,
        0, 0, (zNear + zFar) / (zNear - zFar), -2 * zNear * zFar / (zFar - zNear),
        0, 0, -1, 0;
    return mat;
}

Mat4 viewPortMatrix(double width, double height) {
    Mat4 mat;
    double w_ = width / 2.0;
    double h_ = height / 2.0;
    double z_ = depth / 2.0;
    mat << w_, 0, 0,  w_,
            0, h_, 0, h_,
            0, 0, z_, z_,
            0, 0, 0, 1;
    return mat;
}

// 求出重心坐标
Vec3 barycentric(Node* nodes, Vec3 p) {
    double xa = nodes[0].screen_coords.x();
    double xb = nodes[1].screen_coords.x();
    double xc = nodes[2].screen_coords.x();
    double ya = nodes[0].screen_coords.y();
    double yb = nodes[1].screen_coords.y();
    double yc = nodes[2].screen_coords.y();
    // ASSERT(((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya) != 0, "error");
    double gamma = ((ya - yb) * p.x() + (xb - xa) * p.y() + xa * yb - xb * ya) / ((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya);
    if(std::abs(gamma) < 1e-5) { return Vec3{-1, 1, 1}; } 
    // ASSERT( ((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya) != 0, "error");
    double beta = ((ya - yc) * p.x() + (xc - xa) * p.y() + xa * yc - xc * ya) / ((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya);
    double alpha = 1 - beta - gamma;
    return Vec3{alpha, beta, gamma};
}
// bounding box
std::array<int, 4> boundBox(Node* nodes) {
    std::array<int, 4> res;
    res[0] = std::floor(std::min(std::min(nodes[0].screen_coords.y(), nodes[1].screen_coords.y()), nodes[2].screen_coords.y()));
    res[1] = std::ceil(std::max(std::max(nodes[0].screen_coords.y(), nodes[1].screen_coords.y()), nodes[2].screen_coords.y()));
    res[2] = std::floor(std::min(std::min(nodes[0].screen_coords.x(), nodes[1].screen_coords.x()), nodes[2].screen_coords.x()));
    res[3] = std::ceil(std::max(std::max(nodes[0].screen_coords.x(), nodes[1].screen_coords.x()), nodes[2].screen_coords.x()));
    return res;
}

bool inCycle(Vec3 vec, Vec3 center, double radius) {
    return std::sqrt((vec.x() - center.x()) * (vec.x() - center.x()) +
         (vec.y() - center.y()) * (vec.y() - center.y())) <= radius;
}

void TestRender::render() {
    glBegin(GL_POINTS);
    // printf("--------------------\n");
    Scene scene;
    Vec3 center{0, 0, 0};
    for(int i = 0; i < width; ++i) {
        for(int j = 0; j < height; ++j) {
            Ray ray(center, Vec3{1.0 * i, 1.0 * j, -255.0});
            TGAColor color = scene.color(ray);
            // if(color[0] != 255) {
            glColor3f(1.0f * color[0] / 255.0f, 1.0 * color[1] / 255.0f, 1.0 * color[2] / 255.0f);
            glVertex3f(1.0f * i / width * 2 - 1, 1.0f * j / height* 2 - 1, 0.0f);
            // }
        }
    }
    glEnd();
}
}