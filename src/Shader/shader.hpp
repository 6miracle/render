#ifndef __RENDER_SHADER_H__
#define __RENDER_SHADER_H__
#include "Engine/Scene/Scene.h"
#include "Engine/camera/camera.h"
#include "util.hpp"
#include <cmath>
#include <memory>
#include <sstream>
#include <unordered_map>
#pragma once

#include "Engine/Model/model.h"
#include "pch.h"
#include "maths/maths.hpp"
#include "tgaimage.h"
#include "Engine/Scene/Scene.h"

namespace render { 
class IShader {
public:
    virtual ~IShader() = default;
    virtual void vertex(Node& node,int face, int nthvert) = 0;
    virtual bool fragment(Node* nodes, Vec3 vec, TGAColor& color) = 0;
    void setUniform(const std::string& name, const Mat4& mat);
    void setModel(Model* model) { model_ = model;}
    void setScene(Scene* scene) {scene_ = scene; }
protected:
    std::unordered_map<std::string, Mat4> map_;
    Model* model_;
    Scene* scene_;

};
// 获取zbuffer的空shader
// class DepthShader: public IShader {
// public:
//   DepthShader(Model* model): model_(model) {}
//   Vec4 vertex(int face, int nthvert) override {
//     return viewportmat * projectionmat * viewportmat * modelmat * local2homo(model_->node(face, nthvert) );
//   }
//   bool fragment(Vec3 vec, TGAColor& color) override {
//     return false;
//   } 
// private:
//   Model* model_;
// };

class TriangleShader:public IShader {
  void vertex(Node& node, int face, int nthvert) {
      model_->Node(node, face, nthvert);
      node.coords = map_["projection"] * map_["view"] * map_["model"] * node.coords;
   }
  bool fragment(Node* node, Vec3 vec, render::TGAColor& color) {
    // LOG_ERROR("%d %d %d", ss.str().c_str());
    color = TGAColor{static_cast<unsigned char>(vec[0] * 255), static_cast<unsigned char>(vec[1] * 255), static_cast<unsigned char>(vec[2] * 255)};
    //  LOG_ERROR("%d %d %d", color[0], color[1], color[2]);
    return false;
  }
};
class CircleShader:public IShader {
  void vertex(Node& node, int face, int nthvert) {
      model_->Node(node, face, nthvert);
      node.coords = map_["projection"] * map_["view"] * map_["model"] * node.coords;
   }
  bool fragment(Node* node, Vec3 vec, render::TGAColor& color) {
    // LOG_ERROR("%d %d %d", ss.str().c_str());
    // color = TGAColor{static_cast<unsigned char>(255.0 * vec.x() / width ), 
    //   static_cast<unsigned char>(255.0 * vec.y() / height), static_cast<unsigned char>(255.0 * vec.x() / width)};
    color = TGAColor{255, 255, 255, 255};
    //  LOG_ERROR("%d %d %d", color[0], color[1], color[2]);
    return false;
  }
};
class RayShader: public render::IShader {
public:
   void vertex(Node& node, int face, int nthvert) {
      model_->Node(node, face, nthvert);
      node.coords = map_["projection"] * map_["view"] * map_["model"] * node.coords;
   }
  bool fragment(Node* node, Vec3 vec, render::TGAColor& color) {
      //   double y = (node[0].screen_coords.y() * vec.x() +  node[1].screen_coords.y() * vec.y() +  node[2].screen_coords.y() * vec.z()) / height;
      // color = scene_->color(y);
      return false;
  }

private:

};

// Gouraud shading + blinn-phong反射模型, 根据每个顶点发现求出顶点颜色然后插值
class GouraudShader: public render::IShader {
public:
    // GouraudShader(Model& model): model_(model) {
    // }

    void vertex(Node& node, int face, int nthvert) {
      model_->Node(node, face, nthvert);
      // Vec4 node = render::local2homo(model_->node(face, nthvert));
      // uvs[nthvert] = model_->uv(face , nthvert);
      uniform_M = map_["view"] * map_["model"];
      uniform_MIT = (map_["projection"] * map_["view"] * map_["model"]).invert().transpose();
      node.coords = map_["projection"] * map_["view"] * map_["model"] * node.coords;
      // intensity_[nthvert] = std::max(0., model_->normal(face , nthvert) * light_dir);
      // return node;
    }
    bool fragment(Node* node, Vec3 vec, render::TGAColor& color) {
      Vec2 uv = node[0].uv * vec[0] + node[1].uv * vec[1] + node[2].uv * vec[2];
      ObjModel* model = (ObjModel*)(model_);
    //   Vec3 n = homo2local(uniform_MIT * localvhomo(model_->normal(uv))).normalized();
    //   Vec3 l = homo2local(uniform_M * localvhomo(light_dir)).normalized();

    //   Vec3 r = (2. * n * (n * l ) - l).normalized();
      
    //   double diff = std::max(0., n * l);
    //   double specular = pow(std::max(r.z(), 0.), model_->specular(uv));
    //   TGAColor diffuseColor = model_->diffuse(uv);
    //   // color =  model_->diffuse(uv);
    //   for(int i = 0; i < 3; ++i) color[i] = std::min(255.,(5 + diffuseColor[i] * (3 * diff + 6 * specular)));
    //   color[3] = 255;
      // for(int i = 0; i < 4; ++i) color[i] = 255;
      color = model->diffuse(uv);
      return false;
    } 
private:
      Vec3 intensity_;
      Vec2 uvs[3];
      Mat4 uniform_M;
      Mat4 uniform_MIT;
};


// // Gohand shader+ blinn phong光照模型 + 切线空间
// class PhongShader:public IShader {
// public:
//   PhongShader(Model* model): model_(model) {
//   }
//   Vec4 vertex(int face, int nthvert) override {
//     triangle_[nthvert] = local2homo(model_->node(face, nthvert));
//     uvs[nthvert] = model_->uv(face, nthvert);
//     uniform_M = map_["view"] * map_["model"];
//     mvp = map_["projectio"] * uniform_M;
//     uniform_MIT = mvp.invert().transpose();
//     return  mvp * triangle_[nthvert];
//   }

//   bool fragment(Vec3 vec, TGAColor& color) override {
//     Vec2 uv = uvs[0] * vec[0] + uvs[1] * vec[1] + uvs[2] * vec[2];
//      Vec3 normalTan = model_->normalTan(uv).normalized();
//     // Vec3 normalTan = Vec3{c[2] / 255.0 * 2.0 - 1 , c[1] / 255.0 * 2.0 - 1, c[0] / 255.0 * 2.0 - 1}.normalized();
//     double u0 = uvs[1].x() - uvs[0].x();
//     double u1 = uvs[2].x() - uvs[0].x();
//     double v0 = uvs[1].y() - uvs[0].y();
//     double v1 = uvs[2].y() - uvs[0].y();
//     Vec3 e0 = { (uvs[1] - uvs[0]).x(), (uvs[1] - uvs[0]).y(), 0.};
//     Vec3 e1 = { (uvs[2] - uvs[0]).x(), (uvs[2] - uvs[0]).y(), 0.};
//     render::Mat<2, 2> mat1{v1, -v0, -u1, u0};
//     render::Mat<2, 3> mat2{e0.x(), e0.y(), e0.z(), e1.x(), e1.y(), e1.z()};
//     render::Mat<2, 3> mat = mat1 * mat2 / (u0 * v1 - v0 * u1);
//     Vec3 t{mat[0][0], mat[0][1], mat[0][2]};
//     Vec3 b{mat[1][0], mat[1][1], mat[1][2]};
//     t.normalized();
//     b.normalized();
//     Vec3 normal = t.cross(b).normalized();

//     Mat4 matrix;
//     matrix << t.x(), b.x(), normal.x(), 0,
//               t.y(), b.y(), normal.y(), 0,
//               t.z(), b.z(), normal.z(), 0,
//               0, 0, 0, 1;
//     Vec4 nor = (matrix * Vec4{normalTan.x(), normalTan.y(), normalTan.z(), 0.}).normalized();
//     Vec3 n = homo2local(uniform_MIT * nor).normalized();
//     Vec3 l = homo2local(uniform_M * localvhomo(light_dir)).normalized();

//     Vec3 r = (2. * n * (n * l ) - l).normalized();
    
//     double diff = std::max(0., n * l);
//     double specular = pow(std::max(r.z(), 0.), model_->specular(uv));
//     TGAColor diffuseColor = model_->diffuse(uv);
//     for(int i = 0; i < 4; ++i) color[i] = std::min(255.,(5 + diffuseColor[i] * (diff + 0.3 * specular)));
//     // color[3] = 255;
//     // double intensity = std::max(0.0, n * light_dir);
//     // color = model_->diffuse(uv) * intensity;
//     return false;
//   }

// private:
//   Model* model_;
//   Vec2 uvs[3];
//   Vec3 normals[3];
//   Vec4 triangle_[3];
//   Mat4 uniform_M;
//   Mat4 uniform_MIT;
//   Mat4 mvp;
// };


// class ShadowShader: public IShader {
// public:
//   ShadowShader(Model* model, Mat4 mvp_light, render::TGAImage& buffer)
//   : model_(model), mvpLight_(mvp_light), buffer_(buffer) {
//   }
//    virtual Vec4 vertex(int face, int nthvert) {
//       Vec4 node = render::local2homo(model_->node(face, nthvert));
//       triangles[nthvert] = node;
//       uvs[nthvert] = model_->uv(face , nthvert);
//       uniform_M = map_["view"] * map_["model"];
//       Mat4 mvp = map_["projection"] * uniform_M;
//       uniform_MIT = mvp.invert().transpose();
//       return mvp * node;
//   }
//   virtual bool fragment(Vec3 vec, render::TGAColor& color)  {
//       Vec2 uv = uvs[0] * vec[0] + uvs[1] * vec[1] + uvs[2] * vec[2];
//       Vec3 n = render::homo2local(uniform_MIT * render::local2homo(model_->normal(uv))).normalized();
//       Vec3 l = render::homo2local(uniform_M * render::local2homo(light_dir)).normalized();
//       // 反射光线
//       Vec3 r = (2 * n * (n* l) - l).normalized();
//       double diff = std::max(0.0, n * l);
//       double specular = pow(std::max(r.z(), 0.), model_->specular(uv));

//       Vec4 light_Vec = mvpLight_ * triangles[0] * vec[0] + mvpLight_ * triangles[1] * vec[1] + mvpLight_ * triangles[2] * vec[2];
//       light_Vec =  light_Vec / light_Vec[3];
//       double depth = light_Vec.z();
//       render::TGAColor cl = model_->diffuse(uv);
//       color = cl;
//       for(int i = 0; i < 3; ++i) {
//         if(depth  > buffer_.get(vec.x(), vec.y()).bgra[0]) {
//           color[i] = 0;
//         } else {
//           color[i] = std::min(255.0, 5.0 + cl[i] * (diff + 6 * specular));
//         }
//       }
//       return false;
//   }
// private:
//   Model* model_;
//    Vec4 triangles[3];
//   Vec3 intensity_;
//   Vec2 uvs[3];
//   Mat4 uniform_M;
//   Mat4 uniform_MIT;
//   render::TGAImage buffer_;
//   Mat4 mvpLight_;

// };


// vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);
// float NDF = DistributionGGX(N, H, roughness);       
// float G   = GeometrySmith(N, V, L, roughness);    
// vec3 nominator    = NDF * G * F;
// float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
// vec3 specular     = nominator / denominator;  
// vec3 kS = F;
// vec3 kD = vec3(1.0) - kS;

// kD *= 1.0 - metallic;   
class PBRShader:public IShader {
public:
  void vertex(Node& node,int face, int nthvert) {
    model_->Node(node, face, nthvert);
    uniform_MV = map_["view"] * map_["model"];
    uniform_IT_MV = uniform_MV.invert().transpose();
    node.coords = map_["projection"] * uniform_MV  * node.coords;
  }
  bool fragment(Node* nodes, Vec3 vec, TGAColor& color) {
    Vec3 lightPos[4]{Vec3{0, 0, 1}, Vec3{0, 1, 0}, Vec3{1, 0, 0}, Vec3{1, 1, 1}};
    TGAColor lightColor[4] = {TGAColor{50, 20, 30, 255}, TGAColor{100, 100, 100, 255},
                              TGAColor{255, 255, 255, 255}, TGAColor{200, 100, 200, 255}};
    double roughness = 0.5;
    Vec3 normal = toVec3(uniform_IT_MV * toVec4v((nodes[0].normal * vec[0] + nodes[1].normal * vec[1] + nodes[2].normal * vec[2]).normalized()));
    Vec4 view_node = map_["projection"].invert() * (nodes[0].coords * vec[0] + nodes[1].coords * vec[1] + nodes[2].coords * vec[2]);
    Vec3 view = toVec3(-1 * view_node).normalized();
   
    // Vec3 F0 = Vec3(0.04); // 基础反射率
    // int metallic = 0;
    // Vec3 albedo = {255, 255, 255};
    // F0 = mix(F0, albedo, metallic);

    double F0 = 0.04;
    double metallic = 0;
    double albedo = 255;
    F0 = mix(F0, albedo, metallic);
    // 出射光线的辐射率/ reflectance equation / BRDF
    Vec3 lo = Vec3(0.0);
    // 四个方向的光
    for(int i = 0; i < 4; ++i) {
      //  calculate per-light radiance
      Vec3 L = (lightPos[i] - toVec3(view_node)).normalized(); // 光线照射方向
      Vec3 H = (view + L).normalized(); // 半程向量
      double distance = (lightPos[i] - toVec3(view_node)).norm();  // 距离
      double attenuation = 1 / (distance * distance); // 光线衰减率
      Vec3 radiance = lightColor[i].toVec3() * attenuation;

      // Cook-Torrance BRDF
      double NDF = DistributionGG(normal, H, roughness);
      double G = GeometrySmith(normal, view, L, roughness);
      double F = fresnelSchlick(clamp(H * view, 0.0, 1.0), F0);

      double numerator = NDF * G * F;
      double denominator = 4.0 * std::max(normal * view, 0.0) * std::max(normal * L, 0.0) + 0.0001;
      double specular = numerator / denominator;

      double kS = F;
      double kD = (1.0 - kS) * (1.0 - metallic);
      double NdotL = std::max(normal * L, 0.0);

      lo += (kD * albedo / std::numbers::pi + specular) * radiance * NdotL;
      
    }

    // ================ambient IBL
    // Vec3 F = fresnelSchlick_IBL(std::max(normal * view, 0.), F0, roughness);
    // Vec3 kS = F;    
    // Vec3 kD = (Vec3(1.0) - kS) * (1.0 - metallic);

    // TGAColor irradiance = ((BallModel*)model_)->irradianceMap(normal);
    // TGAColor diffuse = irradiance * albedo;

    // const float MAX_REFLECTION_LOD = 4.0;
    // TGAColor prefilterColor = ((BallModel*)model_)->prefilterMap(,roughness * MAX_REFLECTION_LOD);


    Vec3 ambient = Vec3(0.03) * albedo; // *ao




    Vec3 vcolor = ambient + lo;
    // HDR tonemapping
    vcolor = vcolor / (vcolor + Vec3(1.0));
    // gamma correct
    vcolor = vcolor.pow(1.0 / 2.2);  

    for(int i = 0; i < 3; ++i) {
      color[i] = vcolor[2 - i];
    }
    return false;
  }
private:  
  Mat4 uniform_MV;
  Mat4 uniform_IT_MV;
};

static int getFace(Vec3 dir) {
  int Max = 0;
  for(int i = 1; i < 3; ++i) {
    if(std::abs(dir[i]) > std::abs(dir[Max])) { Max = i; }
  }
  return dir[Max] >= 0 ? Max : Max + 1;
}

class SkyBoxShader: public IShader {
public:
    void vertex(Node& node,int face, int nthvert) {
      model_->Node(node, face, nthvert);
      // uniform_MV = map_["view"] * map_["model"];
      // uniform_IT_MV = uniform_MV.invert().transpose();
      node.coords = map_["projection"] * map_["view"] * map_["model"] * node.coords;
    }
    bool fragment(Node* nodes, Vec3 vec, TGAColor& color) {
      Vec3 center;
      Vec2 uvs[3];
      TGAColor colors[3];
      for(int i = 0; i < 3; ++i) {
        Vec3 dir = (toVec3(nodes[i].screen_coords) - center).normalized();
        int face = getFace(dir);
        colors[i] = ((BallModel*)model_)->cubeMap(face, dir);
      }
      Vec2 uv = uvs[0] * vec[0] +  uvs[1] * vec[1] + uvs[2] * vec[2];
      color = colors[0] * vec[0] + colors[1] * vec[1] + colors[2] * vec[2];
      return false;
    }

private:
};

class IrradianceShader:public IShader {
public:
   void vertex(Node& node, int face, int nthvert) {
      model_->Node(node, face, nthvert);
      node.coords = map_["projection"] * map_["view"] * map_["model"] * node.coords;
   }
  bool fragment(Node* nodes, Vec3 vec, render::TGAColor& color) {
      Vec3 normal = toVec3((map_["view"] * map_["model"]).invert().transpose()
         * toVec4v((nodes[0].normal * vec[0] + nodes[1].normal * vec[1] + nodes[2].normal * vec[2]).normalized()));
      TGAColor irradiance;
      // 向量坐标空间
      Vec3 up{0.0, 1.0, 0.0};
      Vec3 right = up.cross(normal).normalized();
      up = normal.cross(right).normalized();

      double sampleData = 0.025;
      double sampleNum = 0.0;

      for(double phi = 0.0; phi < 2.0 * std::numbers::pi; phi += sampleData) {
        for(double theta = 0.0; theta < 0.5 * std::numbers::pi; theta += sampleData) {
          Vec3 tangentSample = Vec3{std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta)};
          Vec3 sampleVec = tangentSample.x() * right + tangentSample.y() * up + tangentSample.z() * normal;

          int face = getFace(sampleVec);
          irradiance = irradiance +  ((BallModel*)model_)->cubeMap(face, sampleVec) * std::cos(theta) * std::sin(theta);
          sampleNum++;
        }
      }

      irradiance =  irradiance * std::numbers::pi * (1.0 / sampleNum);
      color = irradiance; 

      return false;
  }

private:
};

// 重要性采样 蒙特卡洛方法Hammersley 序列 
inline float RadicalInverse_VdC(int bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
// 获得采样向量
inline Vec2 Hammersley(int i, int N) {
    return Vec2{double(i)/double(N), RadicalInverse_VdC(i)};
}  

inline Vec3 ImportanceSampleGGX(Vec2 Xi, Vec3 N, float roughness) {
  double a = roughness * roughness;

  double phi = 2.0 * std::numbers::pi * Xi.x();
  double cosTheta = sqrt((1.0 - Xi.y()) / 1.0 + (a * a - 1.0) * Xi.y());
  double sinTheta = sqrt(1. - cosTheta * cosTheta);

  Vec3 H;
  H[0] = std::cos(phi) * sinTheta;
  H[1] = std::sin(phi) * sinTheta;
  H[2] = cosTheta;

  Vec3 up = std::abs(N.z()) < 0.999 ? Vec3{0., 0., 1.} : Vec3{1., 0., 0.};
  Vec3 tangent = up.cross(N).normalized();
  Vec3 bitangent = N.cross(tangent).normalized();

  Vec3 sampleVec = tangent * H.x() + bitangent * H.y() + N * H.z();
  return sampleVec.normalized();
}

class PreFilterShader:public IShader {
public:
   void vertex(Node& node, int face, int nthvert) {
      model_->Node(node, face, nthvert);
      node.coords = map_["projection"] * map_["view"] * map_["model"] * node.coords;
   }
  bool fragment(Node* nodes, Vec3 vec, render::TGAColor& color) {
      // materail
      double roughness = 0.5;

      Vec3 normal = toVec3((map_["view"] * map_["model"]).invert().transpose()
         * toVec4v((nodes[0].normal * vec[0] + nodes[1].normal * vec[1] + nodes[2].normal * vec[2]).normalized()));
      Vec3 R = normal;
      Vec3 v = R;

      const int sampleNum = 1024u;
      TGAColor prefilteredColor;
      double totalWeight = 0.;

      for(int i = 0; i < sampleNum; ++i) {

        // 获得wi, 即入射光线
        Vec2 Xi = Hammersley(i, sampleNum);
        Vec3 H = ImportanceSampleGGX(Xi, normal, roughness);
        Vec3 L = (2. * (v * H) * H - v).normalized();
        // Vec3 l = 
        double nl = std::max(0., normal * L);

        if(nl > 0.0) {
          double d = DistributionGG(normal, H, roughness);
          double nh = std::max(normal * v, 0.0);
          double nv = std::max(v * H, 0.0);
          double pdf = d * nh / (4 * nv) + 0.0001;

          double resolution = 512.0; 
          double saTexel = 4. * std::numbers::pi / (6.0 * resolution * resolution);
          double saSample = 1.0 / (sampleNum * pdf + 0.0001);
          double mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

          // TODO:level 分层
          prefilteredColor = prefilteredColor + ((BallModel*)model_)->cubeMap(getFace(L) ,L) * nl; // 暂时不分层
          totalWeight += nl;
        }

      }

      for(int i = 0; i < 3; ++i) {
        prefilteredColor[i] = prefilteredColor[i] / totalWeight;
      }
      color = prefilteredColor;
      return false;
  }

private:
};


class InterateBRDFSHader:public IShader {
public:
   void vertex(Node& node, int face, int nthvert) {
      model_->Node(node, face, nthvert);
      node.coords = map_["projection"] * map_["view"] * map_["model"] * node.coords;
   }
  bool fragment(Node* node, Vec3 vec, render::TGAColor& color) {
    // 设置
      double roughness = 0.5;
      double nv = 0.5;

      Vec3 V{std::sqrt(1. - nv * nv), 0., nv};

      double A = 0.0, B = 0.0;
      Vec3 N = Vec3{0.0, 0.0, 1.0};

      const int sampleNum = 1024;
      for(int i = 0;i < sampleNum; ++i) {
        Vec2 Xi = Hammersley(i,sampleNum);
        Vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        Vec3 L = (2.0 * (V * H) *H - V).normalized();

        double nl = std::max(L.z(), 0.);
        double nh = std::max(H.z(), 0.);
        double vh = std::max(V * H, 0.0);

        if(nl > 0.0) {
          double G = GeometrySmith(N, V, L, roughness);
          double G_Vis = (G * vh) / (nh * nv);
          double fc = std::pow(1 - vh, 5.);

          A += (1.0 - fc) * G_Vis;
          B += fc * G_Vis; 
        }
      }
      A /= sampleNum;
      B /= sampleNum;

      color[0] = A;
      color[1] = B;
      return false;
  }

private:
};

//=================IBL==================
class IBLShader:public IShader {
public:
  void vertex(Node& node,int face, int nthvert) {
    model_->Node(node, face, nthvert);
    uniform_MV = map_["view"] * map_["model"];
    uniform_IT_MV = uniform_MV.invert().transpose();
    node.coords = map_["projection"] * uniform_MV  * node.coords;
  }
  bool fragment(Node* nodes, Vec3 vec, TGAColor& color) {
    Vec3 lightPos[4] = {Vec3{0, 0, 1}, Vec3{0, 1, 0}, Vec3{1, 0, 0}, Vec3{1, 1, 1}};
    TGAColor lightColor[4] = {TGAColor{50, 20, 30, 255}, TGAColor{100, 100, 100, 255},
                              TGAColor{255, 255, 255, 255}, TGAColor{200, 100, 200, 255}};

    Vec3 normal = toVec3(uniform_IT_MV * toVec4v((nodes[0].normal * vec[0] + nodes[1].normal * vec[1] + nodes[2].normal * vec[2]).normalized()));
    Vec4 view_node = map_["projection"].invert() * (nodes[0].coords * vec[0] + nodes[1].coords * vec[1] + nodes[2].coords * vec[2]);
    Vec3 view = toVec3(-1 * view_node).normalized();
     Vec3 refl = reflect(view, normal);

    // material
    double metallic = 0;
    double albedo = 255;
    double roughness = 0.5;
    double ao = 1;

    double F0 = 0.04;
    F0 = mix(F0, albedo, metallic);
    // 出射光线的辐射率/ reflectance equation / BRDF
    Vec3 lo = Vec3(0.0);
    // ===============四个方向的光===============
    for(int i = 0; i < 4; ++i) {
      //  calculate per-light radiance
      Vec3 L = (lightPos[i] - toVec3(view_node)).normalized(); // 光线照射方向
      Vec3 H = (view + L).normalized(); // 半程向量
      double distance = (lightPos[i] - toVec3(view_node)).norm();  // 距离
      double attenuation = 1 / (distance * distance); // 光线衰减率
      Vec3 radiance = lightColor[i].toVec3() * attenuation;

      // Cook-Torrance BRDF
      double NDF = DistributionGG(normal, H, roughness);
      double G = GeometrySmith(normal, view, L, roughness);
      double F = fresnelSchlick(clamp(H * view, 0.0, 1.0), F0);

      double numerator = NDF * G * F;
      double denominator = 4.0 * std::max(normal * view, 0.0) * std::max(normal * L, 0.0) + 0.0001;
      double specular = numerator / denominator;

      double kS = F;
      double kD = (1.0 - kS) * (1.0 - metallic);
      double NdotL = std::max(normal * L, 0.0);

      lo += (kD * albedo / std::numbers::pi + specular) * radiance * NdotL;
      
    }
    double F = fresnelSchlick_IBL(std::max(normal * view, 0.), F0, roughness);
    double kS = F;
    double kD = (1.0 - kS) * (1.0 - metallic);

    TGAColor irradiance = ((BallModel*)model_)->irradianceMap(normal);
    TGAColor diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    TGAColor prefilterColor = ((BallModel*)model_)->prefilterMap(refl,(int)(roughness * MAX_REFLECTION_LOD));
    TGAColor brdf = ((BallModel*)model_)->brdfLUT(Vec2{std::max(normal * view, 0.), roughness});
    
    TGAColor specluar =  prefilterColor * (F * brdf.bgra[0] + brdf.bgra[1]);
    Vec3 ambient = (kD * diffuse.toVec3() + specluar.toVec3()) * ao;
    Vec3 vcolor = ambient + lo;

    // HDR tonemapping
    vcolor = vcolor / (vcolor + Vec3(1.0));
    // gamma correct
    vcolor = vcolor.pow(1.0 / 2.2);  

    for(int i = 0; i < 3; ++i) {
      color[i] = vcolor[2 - i];
    }
    return false;
  }
private:  
  Mat4 uniform_MV;
  Mat4 uniform_IT_MV;
};
}

#endif