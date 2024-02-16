#ifndef __RENDER_IMAGE_HPP__
#define __RENDER_IMAGE_HPP__


// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#include "util.hpp"
#ifdef _MSC_VER
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_STATIC 
#include "stb_image/stb_image.hpp"

#include "pch.h"

class Image {
public:
    Image() = default;
    Image(const std::string& filename) {
        if(load(filename)) { return ; }
        std::cout << "Load image fail" << '\n';;
    }
    ~Image() { STBI_FREE(data_); }

    bool load(const std::string& filename) {
        int n = bpp_;
        data_ = stbi_load(filename.c_str(), &w_, &h_, &n, bpp_);
        bytes_per_scanline = bpp_ * w_;
        return data_ != nullptr;
    }

    int width() const { return (data_ == nullptr) ? 0 : w_; }
    int height() const { return (data_ == nullptr) ? 0 : h_; }

    const unsigned char* get(int x, int y) const {
        // clamp()
        // LOG_ERROR("%d %d", x, y);
        static unsigned char magenta[] = { 255, 0, 255 };
        if (data_ == nullptr) return magenta;

        x = clamp(x, 0, w_);
        y = clamp(y, 0, h_);

        return data_ + y * bytes_per_scanline + x * bpp_;
    }
private:
    int w_;
    int h_;
    int bpp_ = 3;
    int bytes_per_scanline;
    unsigned char* data_;

    static int clamp(int x, int low, int high) {
        if(x < low) { return low; }
        if(x < high) { return x; }
        return high - 1;
    }
};


// Restore MSVC compiler warnings
#ifdef _MSC_VER
    #pragma warning (pop)
#endif
#endif