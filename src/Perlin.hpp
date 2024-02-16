#ifndef __RENDER_PERLIN_HPP__
#define __RENDER_PERLIN_HPP__

#include "maths/maths.hpp"
#include "pch.h"
#include "util.hpp"
namespace  render {

// 柏林噪声 https://zhuanlan.zhihu.com/p/206271895
class Perlin {
public:
    Perlin() {
        ranVec = new Vec3[point_count];
        for(int i = 0; i < point_count; ++i) {
            ranVec[i] = random_Vec3(-1, 1).normalized();
        }

        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();

    }

    ~Perlin() {
        // delete []ranfloat;
        delete []ranVec;
        delete []perm_x;
        delete []perm_y;
        delete []perm_z;
    }

    // "随机数字块"
    // double noise(const Vec3& p) const {
    //     int i = static_cast<int>(4 * p.x()) & 255;
    //     int j = static_cast<int>(4 * p.y()) & 255;
    //     int k = static_cast<int>(4 * p.z()) & 255;
    //     return ranfloat[perm_x[i] ^ perm_x[j] ^ perm_z[k]];
    // }


   // 平滑处理结果
    // double noise(const Vec3& p) const {
    //     auto u = p.x() - std::floor(p.x());
    //     auto v = p.y() - std::floor(p.y());
    //     auto w = p.z() - std::floor(p.z());
    //     // 埃尔米特平滑改进, 对插值舍入， 避免马赫带()
    //     u = u * u * (3 - 2 * u);
    //     v = v * v * (3 - 2 * v);
    //     w = w * w * (3 - 2 * w);
    //     auto i = static_cast<int>(floor(p.x()));
    //     auto j = static_cast<int>(floor(p.y()));
    //     auto k = static_cast<int>(floor(p.z()));
    //     double c[2][2][2];
    //     for(int di = 0; di < 2; ++di) {
    //         for(int dj = 0; dj < 2; ++dj) {
    //             for(int dk = 0; dk < 2; dk ++) {
    //                 c[di][dj][dk] = ranfloat[
    //                     perm_x[(i + di) & 255] ^ 
    //                     perm_x[(j + dj) & 255] ^ 
    //                     perm_z[(k + dk) & 255]
    //                 ];
    //             }
    //         }
    //     }
    //     return trilinear_interp(c, u, v, w);
    // }

    // 随机向量改进
     double noise(const Vec3& p) const {
        auto u = p.x() - std::floor(p.x());
        auto v = p.y() - std::floor(p.y());
        auto w = p.z() - std::floor(p.z());
        // 埃尔米特平滑改进, 对插值舍入， 避免马赫带()
        // u = u * u * (3 - 2 * u);
        // v = v * v * (3 - 2 * v);
        // w = w * w * (3 - 2 * w);
        auto i = static_cast<int>(floor(p.x()));
        auto j = static_cast<int>(floor(p.y()));
        auto k = static_cast<int>(floor(p.z()));
        Vec3 c[2][2][2];
        for(int di = 0; di < 2; ++di) {
            for(int dj = 0; dj < 2; ++dj) {
                for(int dk = 0; dk < 2; dk ++) {
                    c[di][dj][dk] = ranVec[
                        perm_x[(i + di) & 255] ^ 
                        perm_y[(j + dj) & 255] ^ 
                        perm_z[(k + dk) & 255]
                    ];
                }
            }
        }
        // return trilinear_interp(c, u, v, w);
        return perlin_interp(c, u, v, w);
    }


    // 湍流Turbulence, 多个叠加频率的复合噪声
    double turb(const Vec3& p, int depth = 7) const {
        double accum = 0.0;
        Vec3 temp_p = p;
        double weight = 1.0;

        for(int i = 0; i < depth; ++i) {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p = temp_p * 2;
        }
        return std::fabs(accum);
    }
private:
    static constexpr int point_count = 256;
    // double* ranfloat; // 梯度表
    Vec3* ranVec;  // 梯度表
    // 排列表
    int* perm_x;  
    int* perm_y;
    int* perm_z;


    static int * perlin_generate_perm() {
        auto p = new int[point_count];
        for(int i = 0; i < point_count; ++i) {
            p[i] = i;
        }
        permute(p, point_count);
        return p;
    }

    static void permute(int* p, int n) {
        for(int i = n - 1; i > 0; i--) {
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    static double trilinear_interp(double c[2][2][2], double u, double v, double w) {
        double accm = 0.0;
        for(int i = 0; i < 2; ++i) {
            for(int j = 0; j < 2; ++j) {
                for(int k = 0; k < 2; ++k) {
                    accm += (i * u + (1 - i) * (1 - u)) *
                            (j * v + (1 - j) * (1 - v)) *
                            (k * w + (1 - k) * (1 - w)) * c[i][j][k];
                }
            }
        }
        return accm;
    }

    static double perlin_interp(Vec3 c[2][2][2], double u, double v, double w) {
        double uu = u * u * (3 - 2 * u);
        double vv = v * v * (3 - 2 * v);
        double ww = w * w * (3 - 2 * w);
         double accm = 0.0;
        for(int i = 0; i < 2; ++i) {
            for(int j = 0; j < 2; ++j) {
                for(int k = 0; k < 2; ++k) {
                    Vec3 weight{u - i, v - j, w - k};
                    accm += (i * uu + (1 - i) * (1 - uu)) *
                            (j * vv + (1 - j) * (1 - vv)) *
                            (k * ww + (1 - k) * (1 - ww)) * (c[i][j][k] * weight);
                }
            }
        }
        return accm;
    } 
};
}

#endif