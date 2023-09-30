#ifndef __RENDER_MATHS_H__
#define __RENDER_MATHS_H__
// #include "pch.h"
#include "util.h"
#include <array>
#include <cmath>
#include <complex>
#include <initializer_list>
#include <ostream>
namespace render {
    
template<size_t column, size_t row> 
class Mat;

template<size_t N>
class Vec {
public:
    Vec() = default;
    Vec(const std::initializer_list<double>& lists);
    Vec(const std::initializer_list<int>& lists);
    // 转置
    Mat<N, 1> Transpose();
    // 求角度
    double angle(Vec<N> v);
    
    auto cross(Vec<N>);

    // 归一化
    double norm() const { return std::sqrt(norm2()); }
    double norm2() const { return  (*this) * (*this);}
    Vec<N> normalized() const { return *this / norm(); }

    double* data()  { return array_; }
    // 维度
    constexpr size_t dimension() const { return N; }


    // 运算符重载
    double& operator[](int i)  {
        ASSERT(i < N, "访问越界");
        return array_[i];
    }
    Vec<N> operator-(const Vec<N>& vec);
    Vec<N> operator+(const Vec<N>& vec);
    Vec<N>& operator+=(const Vec<N>& vec);
    Vec<N>& operator-=(const Vec<N>& vec);
    Vec<N>& operator/=(double val);

    double x() const;
    double y() const;
    double z() const;

private:
    double array_[N] = {0};
};


template <size_t N>
Vec<N>::Vec(const std::initializer_list<double>& lists) {
   size_t i = 0;
   for(auto it = lists.begin(); it != lists.end() && i < N; ++it, ++i) {
        array_[i] = *it;
   }
}
template <size_t N>
Vec<N>::Vec(const std::initializer_list<int>& lists) {
   size_t i = 0;
   for(auto it = lists.begin(); it != lists.end() && i < N; ++it, ++i) {
        array_[i] = *it;
   }
}
// 转置
template<size_t N>
Mat<N, 1> Vec<N>::Transpose() {
    Mat<N, 1> mat;
    for(int i = 0; i < N; ++i) {
        mat << array_[i];
    }
    return mat;
}

// 求角度
template<size_t N>
double Vec<N>::angle(Vec<N> v) {
    double res = ((*this) * v ) / (std::sqrt(norm2() + v.norm2()));
    return std::acos(res);
}
// 叉乘
template<size_t N>
auto Vec<N>::cross(Vec<N> v) {
    ASSERT("error", "方法未实现");
}
template<>
inline auto Vec<2>::cross(Vec<2> v) {
    return array_[0] * v.array_[1] - array_[1] * v.array_[0];
}
template<>
inline auto Vec<3>::cross(Vec<3> v) {
    Vec<3> vec = {array_[1] * v.array_[2] - array_[2] * v.array_[1],
            array_[2] * v.array_[0] - array_[0] * v.array_[2],
            array_[0] * v.array_[1] - array_[1] * v.array_[0]
            };
    return vec;
}

// 运算
template<size_t N>
double operator*(Vec<N> v1, Vec<N> v2) {
    double result = 0;
    double* array1 = v1.data();
    double* array2 = v2.data();
    for(size_t i = 0; i < N; ++i) {
        result += array1[i] * array2[i]; 
    }
    return result;
}
// 加法
template<>
inline Vec<2> Vec<2>::operator+(const Vec<2>& vec) {
    return {array_[0] + vec.array_[0], array_[1] + vec.array_[1]};
}
template<size_t N>
Vec<N> Vec<N>::operator+(const Vec<N>& vec) {
    Vec<N> res;
    for(size_t i = 0; i < N; ++i) {
        res[i] = array_[i] + vec.array_[i];
    }   
    return res;
}
template<size_t N>
Vec<N>& Vec<N>::operator+=(const Vec<N>& vec) {
    for(size_t i = 0; i < N; ++i) {
        array_[i] += vec.array_[i];
    }
    return *this;
}

// 减法
template<size_t N>
Vec<N>& Vec<N>::operator-=(const Vec<N>& vec) {
    for(size_t i = 0; i < N; ++i) {
        array_[i] -= vec.array_[i];
    }
    return *this;
}

template<>
inline Vec<2> Vec<2>::operator-(const Vec<2>& vec) {
    return {array_[0] - vec.array_[0], array_[1] - vec.array_[1]};
}
template<size_t N>
Vec<N> Vec<N>::operator-(const Vec<N>& vec) {
    Vec<N> res;
    for(size_t i = 0; i < N; ++i) {
        res[i] = array_[i] - vec.array_[i];
    }   
    return res;
}

// 除法
template<size_t N> 
Vec<N> operator/(Vec<N> v1, double val) {
    ASSERT(val != 0, "divide by zero error");
    Vec<N> vec;
    for(int i = 0; i < N; ++i) {
        vec[i] = v1[i] / val;
    }
    return vec;
}
template<size_t N> 
Vec<N>& Vec<N>::operator/=(double val) {
    ASSERT(val != 0, "divide by zero error");
    for(int i = 0; i < N; ++i) {
        array_[i] /= val;
    }
    return *this;
}
template<size_t N> 
Vec<N> operator*(Vec<N> v1, double val) {
    Vec<N> vec;
    for(int i = 0; i < N; ++i) {
        vec[i] = v1[i] * val;
    }
    return vec;
}
template<size_t N> 
Vec<N> operator*(double val, Vec<N> v1) {
    Vec<N> vec;
    for(int i = 0; i < N; ++i) {
        vec[i] = v1[i] * val;
    }
    return vec;
}

template <size_t N>
std::ostream& operator<<(std::ostream& os, Vec<N> v) {
    for(size_t i = 0; i < N; ++i) {
        os << v[i];
        if(i != N - 1) os << " ";
    }
    return os;
}


template <size_t N>
double Vec<N>::x() const {
    ASSERT(N >= 1 , "N >= 1");
    return array_[0];
}
template <size_t N>
double Vec<N>::y() const {
    ASSERT(N >= 2 , "N >= 2");
    return array_[1];
}
template <size_t N>
double Vec<N>::z() const {
    ASSERT(N >= 3 , "N >= 3");
    return array_[2];
}


using Vec2 = Vec<2>;
using Vec3 = Vec<3>;
using Vec4 = Vec<4>;

inline Vec4 local2homo(Vec3 vec) {
    return {vec[0], vec[1], vec[2], 1.0};
}
inline Vec3 homo2local(Vec4 vec) {
    return {vec[0], vec[1], vec[2]};
}
inline Vec4 localvhomo(Vec3 vec) {
    return {vec[0], vec[1], vec[2], 0.0};
}

}
#endif