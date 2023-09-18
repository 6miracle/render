#pragma once

#include "Engine/Model/model.h"
#include "pch.h"
#include "maths/maths.hpp"
#include "Engine/gl.h"
#include "tgaimage.h"

namespace render { 

// 获取zbuffer的空shader
class DepthShader: public IShader {
public:
  DepthShader(Model* model): model_(model) {}
  Vec4 vertex(int face, int nthvert) override {
    return viewportmat * projectionmat * viewportmat * modelmat * local2homo(model_->node(face, nthvert) );
  }
  bool fragment(Vec3 vec, TGAColor& color) override {
    return false;
  } 
private:
  Model* model_;
};


// Gouraud shading + blinn-phong反射模型, 根据每个顶点发现求出顶点颜色然后插值
class GouraudShader: public render::IShader {
public:
    GouraudShader(Model* model): model_(model) {
      uniform_M = viewmat * modelmat;
      uniform_MIT = (projectionmat * viewmat * modelmat).invert().transpose();
    }
    Vec4 vertex(int face, int nthvert) {
      Vec4 node = render::local2homo(model_->node(face, nthvert));
      uvs[nthvert] = model_->uv(face , nthvert);
      return render::viewportmat * render::projectionmat * render::viewmat * render::modelmat* node;
      // intensity_[nthvert] = std::max(0., model_->normal(face , nthvert) * light_dir);
      // return node;
    }
    bool fragment(Vec3 vec, render::TGAColor& color) {
      Vec2 uv = uvs[0] * vec[0] + uvs[1] * vec[1] + uvs[2] * vec[2];
      Vec3 n = homo2local(uniform_MIT * localvhomo(model_->normal(uv))).normalized();
      Vec3 l = homo2local(uniform_M * localvhomo(light_dir)).normalized();

      Vec3 r = (2. * n * (n * l ) - l).normalized();
      
      double diff = std::max(0., n * l);
      double specular = pow(std::max(r.z(), 0.), model_->specular(uv));
      TGAColor diffuseColor = model_->diffuse(uv);
      for(int i = 0; i < 3; ++i) color[i] = std::min(255.,(5 + diffuseColor[i] * (diff + .6 * specular)));
      color[3] = 255;
      return false;
    } 
private:
      Vec3 intensity_;
      Vec2 uvs[3];
      Model* model_;
      Mat4 uniform_M;
      Mat4 uniform_MIT;
};


// Gohand shader+ blinn phong光照模型 + 切线空间
class PhongShader:public IShader {
public:
  PhongShader(Model* model): model_(model) {
    mvp = viewportmat * projectionmat * viewmat * modelmat;
    uniform_MIT = (projectionmat * viewmat * modelmat).invert().transpose();
    uniform_M = viewmat * modelmat;
  }
  Vec4 vertex(int face, int nthvert) override {
    triangle_[nthvert] = local2homo(model_->node(face, nthvert));
    uvs[nthvert] = model_->uv(face, nthvert);
    return mvp * triangle_[nthvert];
  }

  bool fragment(Vec3 vec, TGAColor& color) override {
    Vec2 uv = uvs[0] * vec[0] + uvs[1] * vec[1] + uvs[2] * vec[2];
     Vec3 normalTan = model_->normalTan(uv).normalized();
    // Vec3 normalTan = Vec3{c[2] / 255.0 * 2.0 - 1 , c[1] / 255.0 * 2.0 - 1, c[0] / 255.0 * 2.0 - 1}.normalized();
    double u0 = uvs[1].x() - uvs[0].x();
    double u1 = uvs[2].x() - uvs[0].x();
    double v0 = uvs[1].y() - uvs[0].y();
    double v1 = uvs[2].y() - uvs[0].y();
    Vec3 e0 = { (uvs[1] - uvs[0]).x(), (uvs[1] - uvs[0]).y(), 0.};
    Vec3 e1 = { (uvs[2] - uvs[0]).x(), (uvs[2] - uvs[0]).y(), 0.};
    render::Mat<2, 2> mat1{v1, -v0, -u1, u0};
    render::Mat<2, 3> mat2{e0.x(), e0.y(), e0.z(), e1.x(), e1.y(), e1.z()};
    render::Mat<2, 3> mat = mat1 * mat2 / (u0 * v1 - v0 * u1);
    Vec3 t{mat[0][0], mat[0][1], mat[0][2]};
    Vec3 b{mat[1][0], mat[1][1], mat[1][2]};
    t.normalized();
    b.normalized();
    Vec3 normal = t.cross(b).normalized();

    Mat4 matrix;
    matrix << t.x(), b.x(), normal.x(), 0,
              t.y(), b.y(), normal.y(), 0,
              t.z(), b.z(), normal.z(), 0,
              0, 0, 0, 1;
    Vec4 nor = (matrix * Vec4{normalTan.x(), normalTan.y(), normalTan.z(), 0.}).normalized();
    Vec3 n = homo2local(uniform_MIT * nor).normalized();
    Vec3 l = homo2local(uniform_M * localvhomo(light_dir)).normalized();

    Vec3 r = (2. * n * (n * l ) - l).normalized();
    
    double diff = std::max(0., n * l);
    double specular = pow(std::max(r.z(), 0.), model_->specular(uv));
    TGAColor diffuseColor = model_->diffuse(uv);
    for(int i = 0; i < 4; ++i) color[i] = std::min(255.,(5 + diffuseColor[i] * (diff + 0.3 * specular)));
    // color[3] = 255;
    // double intensity = std::max(0.0, n * light_dir);
    // color = model_->diffuse(uv) * intensity;
    return false;
  }

private:
  Model* model_;
  Vec2 uvs[3];
  Vec3 normals[3];
  Vec4 triangle_[3];
  Mat4 uniform_M;
  Mat4 uniform_MIT;
  Mat4 mvp;
};


class ShadowShader: public IShader {
public:
  ShadowShader(Model* model, Mat4 mvp_light, render::TGAImage& buffer)
  : model_(model), mvpLight_(mvp_light), buffer_(buffer) {
    uniform_M = viewmat * modelmat;
    uniform_MIT = (projectionmat * uniform_M).invert().transpose();
  }
   virtual Vec4 vertex(int face, int nthvert) {
      Vec4 node = render::local2homo(model_->node(face, nthvert));
      triangles[nthvert] = node;
      uvs[nthvert] = model_->uv(face , nthvert);
      return render::viewportmat * render::projectionmat * render::viewmat * render::modelmat* node;
  }
  virtual bool fragment(Vec3 vec, render::TGAColor& color)  {
      Vec2 uv = uvs[0] * vec[0] + uvs[1] * vec[1] + uvs[2] * vec[2];
      Vec3 n = render::homo2local(uniform_MIT * render::local2homo(model_->normal(uv))).normalized();
      Vec3 l = render::homo2local(uniform_M * render::local2homo(light_dir)).normalized();
      // 反射光线
      Vec3 r = (2 * n * (n* l) - l).normalized();
      double diff = std::max(0.0, n * l);
      double specular = pow(std::max(r.z(), 0.), model_->specular(uv));

      Vec4 light_Vec = mvpLight_ * triangles[0] * vec[0] + mvpLight_ * triangles[1] * vec[1] + mvpLight_ * triangles[2] * vec[2];
      light_Vec =  light_Vec / light_Vec[3];
      double depth = light_Vec.z();
      render::TGAColor cl = model_->diffuse(uv);
      color = cl;
      for(int i = 0; i < 3; ++i) {
        if(depth  > buffer_.get(vec.x(), vec.y()).bgra[0]) {
          color[i] = 0;
        } else {
          color[i] = std::min(255.0, 5.0 + cl[i] * (diff + 6 * specular));
        }
      }
      return false;
  }
private:
  Model* model_;
   Vec4 triangles[3];
  Vec3 intensity_;
  Vec2 uvs[3];
  Mat4 uniform_M;
  Mat4 uniform_MIT;
  render::TGAImage buffer_;
  Mat4 mvpLight_;

};
}