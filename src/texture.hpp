#ifndef __RENDER_TEXTURE_HPP__
#define __RENDER_TEXTURE_HPP__

#include "Perlin.hpp"
#include "maths/maths.hpp"
#include "tools/image.hpp"
#include "util.hpp"
#include <memory>
namespace render {

struct TGAColor {
    uint8_t bgra[4]{0};
    uint8_t bytespp = 4;
    TGAColor() = default;
    TGAColor(const std::initializer_list<uint8_t>& list);
    // TGAColor(const std::initializer_list<double>& list);
    uint8_t& operator[](const int i) { return bgra[i]; }
    TGAColor& operator*(double val);
    Vec3 toVec3() const { return Vec3{bgra[2], bgra[1], bgra[0]}; }
    Vec3 toVec() const { return Vec3{bgra[2] / 255.0, bgra[1] / 255.0, bgra[0] / 255.0};}
    Vec4 toVec4() const { return Vec4{bgra[3], bgra[2], bgra[1], bgra[0]}; }
    static TGAColor fromVec3(Vec3 vec) { 
        return TGAColor{(uint8_t)(vec[2] * 256), (uint8_t)(vec[1] * 256), (uint8_t)(vec[0] * 256)}; 
    }
    TGAColor operator+(const TGAColor& color) { 
        TGAColor result;
        for(int i = 0; i < 4; ++i) {
            result.bgra[0] = std::min(bgra[0] +color.bgra[0], 255);
            result.bgra[1] = std::min(bgra[1] +color.bgra[1], 255);
            result.bgra[2] = std::min(bgra[2] +color.bgra[2], 255);
            result.bgra[3] = std::min(bgra[3] +color.bgra[3], 255);
        }
        return result;
     }
};
// 纹理基类
class Texture {
public:
    virtual ~Texture() = default;
    virtual TGAColor get(const int w, const int h) const { return TGAColor{}; }
    virtual Vec3 value(Vec2 uv, const Vec3& p) const { return Vec3{}; }
    // virtual void set(const int w, const int h, const TGAColor& color);
};


// 单一颜色 恒定颜色
class SolidColor: public Texture {
public:
    SolidColor(Vec3 color):color_(color) {}
    TGAColor get(const int w, const int h) const override{
        return TGAColor::fromVec3(color_);
    }
    Vec3 value(Vec2 uv, const Vec3& p) const override { 
        return color_;
    }
private:
    Vec3 color_;
};

// 棋盘格纹理
class CheckerTexture: public Texture {
public:
    CheckerTexture(double scale, std::shared_ptr<Texture> odd, std::shared_ptr<Texture> even):
        inv_scale_(1 / scale), odd_(odd), even_(even) {}
    CheckerTexture(double scale, Vec3 c1, Vec3 c2)
        : inv_scale_(scale), odd_(std::make_shared<SolidColor>(c1)), even_(std::make_shared<SolidColor>(c2)) {}
    // CheckerTexture(double scale, Vec3 v1, Vec3 v2)
    //     : inv_scale_(scale), odd_(std::make_shared<SolidColor>(v1)), even_(std::make_shared<SolidColor>(v2)) {}
    TGAColor get(const int w, const int h) {
        return TGAColor{};
    }
    Vec3 value(Vec2 uv, const Vec3& p) const override {
        auto xInteger = static_cast<int>(std::floor(inv_scale_ * p.x()));
        auto yInteger = static_cast<int>(std::floor(inv_scale_ * p.y()));
        auto zInteger = static_cast<int>(std::floor(inv_scale_ * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;
        // std::cout << (isEven ? "1" : "2" )<<'\n';
        return isEven ? even_->value(uv, p) : odd_->value(uv, p);
    }
    
private:
    double inv_scale_;
    std::shared_ptr<Texture> odd_;  // 奇数
    std::shared_ptr<Texture> even_; // 偶数
};


class NoiseTexture: public Texture {
public:
    NoiseTexture() {}
    NoiseTexture(double sc): scale(sc) {}
    virtual Vec3 value(Vec2 uv, const Vec3& p) const { 
        // return TGAColor{255,255, 255} * 0.5 * ( 1 + noise.turb(scale * p));
        // 调整相位
        auto s = p * scale;
        return Vec3{1, 1, 1} * 0.5 * ( 1 + std::sin(s.z() + 10 * noise.turb(s)));
    }
private:
    Perlin noise;    
    double scale; // 用于设置频率
};


class ImageTexture: public Texture {
public:
    ImageTexture(const std::string& name): image_(name) { } 
    Vec3 value(Vec2 uv, const Vec3& p) const override { 
         // If we have no texture data, then return solid cyan as a debugging aid.
        if (image_.height() <= 0) return Vec3{0,1,1};
        // LOG_ERROR("%lf, %lf",uv.x(), uv.y());   
        uv = Vec2{clamp(uv.x(), 0, 1), 1  - clamp(uv.y(), 0, 1)};
        auto rgb = image_.get(uv.x() * image_.width(), uv.y() * image_.height());
        auto color_scale = 1.0 / 255;
        // std::cout << uv <<'\n';
        // LOG_ERROR("%d, %d, %d", rgb[0], rgb[1], rgb[2]);
        // std::cout << (int)rgb[0] << " " << (int)rgb[1] << " " << (int)rgb[2] <<'\n';
        return Vec3{color_scale * rgb[0], color_scale * rgb[1], color_scale * rgb[2]};
    }
private:    
    Image image_;   
};
}

#endif