#include "tgaimage.h"
#include "util.h"
#include <fstream>
#include <ios>


namespace render {
TGAImage::TGAImage(const int w, const int h, const int bpp)
    : w_(w), h_(h), bpp_(bpp), data_(w * h * bpp, 0) {}

// 读取tga文件
bool TGAImage::read_tga_file(const std::string filename) {
    std::ifstream ifs(filename,std::ios::binary);
    if(!ifs.is_open()) {
        LOG("open file fail");
        return false;
    }
    TGAHeader header;
    ifs.read((char*)&header, sizeof(TGAHeader));

    if(!ifs.good()) {
        LOG("an error occured");
        return false;
    }

    h_ = header.height;
    w_ = header.width;
    bpp_ = header.bitsperpixel >> 3;

    if(w_ <= 0 || h_ <= 0 || (bpp_ != RGB && bpp_ != RGBA && bpp_ != GRAYSCALE)) {
        LOG("bad bpp(or w or h) value ");
        return false;
    }

    size_t nbytes = w_ * h_  * bpp_;
    data_.resize(nbytes, 0);
    // 未压缩的黑白图像 和未压缩的真彩图像
    if(3 == header.datatypecode || 2 == header.datatypecode) {
        ifs.read((char*)data_.data(), data_.size());
        if(!ifs.good()) {
            LOG("an error occured");
            return false;
        }
    } else if(10 == header.datatypecode || 11 == header.datatypecode) {
        if(!load_rle_data(ifs)) {
            LOG("an error occured");
            return false;
        }
    } else {
        LOG("Unknown file format %d", header.colormaptype);
        return false;
    }

    if(!(header.imagedescriptor & 0x20)) {
        flip_vertically();
    }
    if(header.imagedescriptor & 0x10) {
        flip_horizontally();
    }
    return true;
}
bool TGAImage::write_tga_file(const std::string filename, const bool vfilp, const bool rle) const {
    constexpr uint8_t develper_area_ref[4] = {0, 0, 0, 0};
    constexpr uint8_t extension_area_ref[4] = {0, 0, 0, 0};
    constexpr uint8_t footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
    
    std::ofstream os(filename, std::ios::binary);
    if(!os.is_open()) {
        LOG("open file fail");
        return false;
    }
    TGAHeader header;
    memset(&header, 0 , sizeof(header));
    header.width = w_;
    header.height = h_;
    header.bitsperpixel = bpp_ << 3;
    header.datatypecode = ((bpp_ == GRAYSCALE) ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.imagedescriptor = vfilp ? 0x00 :  0x20; 
    os.write((char*)&header, sizeof(header));

    if(!os.good()) {
        LOG("can't dump the tga file");
        return false;
    }

    if(!rle) {
        os.write((const char*)data_.data(), w_ * h_ * bpp_);
        if(!os.good()) {
            LOG("can't unload rle data");
            return false;
        }
    } else if(!write_rle_data(os)){
        LOG("write rle data error");
        return false;
    }


    os.write((const char*)develper_area_ref, sizeof(develper_area_ref));
    os.write((const char*)extension_area_ref, sizeof(extension_area_ref));
    os.write((const char*)footer, sizeof(footer));
    if(!os.good()) {
        LOG("can't dump the tga file");
        return false;
    }
    os.close();
    return true;
}

bool TGAImage::load_rle_data(std::ifstream& in) {
    size_t pixelcount = w_ * h_;
    size_t num = 0;
    size_t rbytes = 0;
    TGAColor color;
    do {
        uint8_t c = in.get();
        if(!in.good()) {
            LOG("an error occured while reading the data\n");
            return false;
        }
        if(c < 128) {
            c++;
            for(uint8_t i = 0; i < c; ++i) {
                in.read((char*)color.bgra, bpp_);
                 if(!in.good()) {
                    LOG("an error occured while reading the data\n");
                    return false;
                }
                for(uint8_t j = 0; j < bpp_; ++j) {
                    data_[rbytes++] = color[j];
                }
                ++num;
                if(num > pixelcount) {
                    LOG("too many pixels read\n");
                    return false;
                }
            }
        } else {
            c -= 127;
            in.read((char*)(color.bgra), bpp_);
             if(!in.good()) {
                LOG("an error occured while reading the data\n");
                return false;
            }
            for(uint8_t i = 0; i < c; ++i) {
                for(uint8_t j = 0; j < bpp_; ++j) {
                    data_[rbytes++] = color[j];
                }
                ++num;
                if(num > pixelcount) {
                    LOG("too many pixels read\n");
                    return false;
                }
            }

        }
    } while(num < pixelcount);
    return true;
}

/*
TGA的RLE压缩方法是以包（packet）为基础进行的。每个包都有一个头字节，其中的最高位（bit 7）表示包的类型
（0表示原始包，1表示RLE包），其余的7位（bit 6-0）表示包含的数据元素数量减一（从0开始计数，所以最多可以有128个元素）。
*/
bool TGAImage::write_rle_data(std::ofstream& out) const {
    constexpr uint8_t max_chunk_length = 128;
    size_t npixels = w_ * h_;
    size_t curpix = 0;
    while(curpix < npixels) {
        size_t chunkstart = curpix * bpp_;
        size_t curbyte = curpix * bpp_;
        uint8_t run_length = 1;
        bool raw = true; 
        while(curpix + run_length < npixels && run_length < max_chunk_length) {
            bool succ_eq = true;   // 是否连续
            for(int t = 0; succ_eq && t < bpp_; t++) {
                succ_eq = (data_[curbyte + t] == data_[curbyte + t + bpp_]);
            }
            curbyte += bpp_;
            if(1 == run_length) { raw = !succ_eq; }
            if(raw && succ_eq) { run_length--; break; }
            if(!raw && !succ_eq) { break; }
            run_length++;
        }
        curpix += run_length;
        out.put(raw ? run_length - 1 : run_length + 127); 
        if(!out.good()) {
            LOG("can't dump the tga file");
            return false;
        }
        // if(raw) {
        //     for(int i = 0; i < run_length * bpp_; ++i) {
        //         std::cout << (int)data_[chunkstart + i] <<" ";
        //     }
        //     std::cout << "\n";
        // }

        out.write((const char*)(data_.data() + chunkstart), (raw ? run_length * bpp_ : bpp_));
        if(!out.good()) {
            LOG("can't dump the tga file");
            return false;
        }
    }
    return true;
}

TGAColor TGAImage::get(const int w, const int h) const {
    if(!data_.size() || w < 0 || h < 0 || w >= w_ || h >= h_) {
        //  LOG("set error, size = %zu w = %d, h = %d", data_.size(), w, h);
        return TGAColor();
    }
    TGAColor color = {0, 0, 0, 0, bpp_};
    size_t nbyte = ( w + h * w_ ) * bpp_;
    for(size_t i = 0; i < bpp_; ++i) {
        color[i] = data_[nbyte + i];
    }
    return color;
}

void TGAImage::set(const int w, const int h,const  TGAColor& color) {
    if(!data_.size() || w < 0 || h < 0 || w >= w_ || h >= h_) {
        // LOG("set error, size = %zu w = %d, h = %d", data_.size(), w, h);
        return ;
    }
    size_t nbyte = (w + h * w_) * bpp_;
    // ::memcpy(data_.data() + nbyte, &color, bpp_);
    for(int i = 0; i < bpp_; ++i) {
        data_[nbyte + i] = color.bgra[i];
        // std::cout << (int)color[i] <<" ";
    }
    // std::cout << "\n";
}

// 水平
void TGAImage::flip_horizontally() {
    for(size_t i = 0; i < w_ / 2; ++i) {
        for(size_t j = 0; j < h_; ++j) {
            for(size_t t = 0; t < bpp_; ++t) {
                std::swap(data_[(i + w_ * j) * bpp_ + t], data_[((j + 1) * w_ - i - 1) * bpp_ + t]);
            }
        }
    }
}

// 竖直
void TGAImage::flip_vertically() {
    for(size_t i = 0; i < w_; ++i) {
        for(size_t j = 0; j < h_ / 2; ++j) {
            for(size_t t = 0; t < bpp_; ++t) {
                std::swap(data_[(j * w_ + i) * bpp_ + t],   data_[((h_ - j - 1) * w_ + i) * bpp_ + t]);
            }
        }
    }
}

int TGAImage::width() const {
    return w_;
}
int TGAImage::height() const {
    return h_;
}


TGAColor& TGAColor::operator*(double val) {
    bgra[0] = (uint8_t)(bgra[0] * val);
    bgra[1] = (uint8_t)(bgra[1] * val);
    bgra[2] = (uint8_t)(bgra[2] * val);
    bgra[3] = (uint8_t)(bgra[3] * val);
    return *this;
}
}