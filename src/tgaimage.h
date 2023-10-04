#ifndef __RENDER_TGAIMAGE_H__
#define __RENDER_TGAIMAGE_H__
#include "pch.h"
#include <fstream>
#include <initializer_list>
#include <stdint.h>
namespace render  {

// TGA扩展文件结构（v2.0）由五部分组成：文件头、图像/颜色表数据、开发者自定义区域、扩展区域和文件尾。
// TGA格式头部
#pragma pack(push, 1) // 设置对齐系数为1
struct TGAHeader {
    uint8_t idlength = 0;  // 图像信息字段长度，取值为0-255,为0时表示没有图像的信息字段
    uint8_t colormaptype = 0; // 颜色表类型
    uint8_t datatypecode = 0; // 图像类型码
    uint16_t colormaporigin = 0; // 颜色表首地址
    uint16_t colormaplength = 0;  // 颜色表长度
    uint8_t colormapdepth = 0; // 颜色表项位数
    uint16_t x_origin = 0; // 图像x坐标的起始位置，左下角x坐标的整型值
    uint16_t y_origin = 0; // 图像y坐标的起始位置，左下角y坐标的整型值
    uint16_t width = 0; // 图像宽度，像素为单位
    uint16_t height = 0; // 图像高度，像素为单位
    uint8_t bitsperpixel = 0; // 每像素占用的存储位数
    uint8_t imagedescriptor = 0; // 图像描述符字节
};
#pragma pack(pop)

struct TGAColor {
    uint8_t bgra[4]{0};
    uint8_t bytespp = 4;
    TGAColor() = default;
    TGAColor(const std::initializer_list<uint8_t>& list);
    uint8_t& operator[](const int i) { return bgra[i]; }
    TGAColor& operator*(double val);
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

class TGAImage {
public:
    enum Format { GRAYSCALE = 1,  RGB = 3, RGBA = 4 };

    TGAImage() = default;
    TGAImage(const int w, const int h, const int bpp);

    bool read_tga_file(const std::string filename);
    bool write_tga_file(const std::string filename, const bool vfilp = true, const bool rle = true) const;
    TGAColor get(const int w, const int h) const;
    void set(const int w, const int h, const TGAColor& color);

    void flip_horizontally();
    void flip_vertically();

    int width() const;
    int height() const;

    void clear() { 
        std::fill(data_.begin(), data_.end(), 0);
    }

    bool empty() { return data_.empty(); }
private:
    bool load_rle_data(std::ifstream& in);
    bool write_rle_data(std::ofstream& out) const;

    int w_;
    int h_;
    uint8_t bpp_;
    std::vector<uint8_t> data_;
};


}
#endif