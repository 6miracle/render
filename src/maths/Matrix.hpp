#ifndef __RENDER_MATRIX_H__
#define __RENDER_MATRIX_H__
#include "maths/maths.hpp"
#include "pch.h"
#include "util.h"
#include <ostream>
namespace render {
template<size_t row, size_t column>
class Mat {
public:
    Mat() = default;
    // 转置
    Mat<column, row> Transpose();

    size_t columns()  { return column; }
    size_t rows() { return rows; }

    Mat<row, column>& operator<<(double val);
    Mat<row, column>& operator<<(Vec<column> vec);
    Mat<row, column>& operator,(double val);
    // template<typename ...Args>
    // Mat<row, column>& operator<<(Args... args);
    Mat<row, column> operator*(double val);
    Mat<row, column> operator/(double val);

    // 运算
    template<size_t num> 
    Mat<row, num> operator*(Mat<column, num>& mat) {
        Mat<row, num> res;
        for(size_t i = 0; i < column; ++i) {
            double result = 0;
            for(size_t j = 0; j < num; ++i) {
                for(size_t t = 0; t < column; ++t) {
                    result += array_[i][j] * mat[i][j]; 
                }
            }
            res << result;
        }
    }

    double* operator[](int i)  {
        ASSERT(i < N, "访问越界");
        return array_[i];
    }
private:
    double array_[row][column] = {0};
    size_t nums_ = 0;
};

// 转置
template<size_t row, size_t column> 
Mat<column, row> Mat<row, column>::Transpose() {
    Mat<column, row> mat;
    for(size_t i = 0; i < column; ++i) {
        for(size_t j = 0; j < row; ++j) {
            mat << array_[i][j];
        }
    }
    return mat;
}


template<size_t row, size_t column>
Mat<row, column>& Mat<row, column>::operator<<(double val) {
    if(nums_ < column * row) {
        array_[nums_ / column][nums_ % column] = val;
        ++nums_;
    }
    return *this;
}
// template<size_t row, size_t column>
// Mat<row, column>& Mat<row, column>::operator<<(int val) {
//     if(nums_ < column * row) {
//         array_[nums_ / column][nums_ % column] = val;
//         ++nums_;
//     }
//     return *this;
// }

template<size_t row, size_t column>
Mat<row, column>& Mat<row, column>::operator<<(Vec<column> vec) {
    if(nums_ < column * row) {
        for(size_t i = 0; i < column; ++i) {
           array_[nums_ / column][nums_ % column] = vec[i];
            ++nums_;
        }
    }
    return *this;
}
template<size_t row, size_t column>
Mat<row, column>& Mat<row, column>::operator,(double val) {
    return operator<<(val);
}

template<size_t row, size_t column>
Mat<row, column> Mat<row, column>::operator*(double val) {
    Mat<row, column> mat;
    for(size_t i = 0; i < row; ++i) {
        for(size_t j = 0; j < column; ++j) {
            mat << array_[i][j] * val;
        }
    }
    return mat;
}

template<size_t row, size_t column>
Mat<row, column> operator*(double val, Mat<row, column> matrix) {
    Mat<row, column> mat;
    for(size_t i = 0; i < row; ++i) {
        for(size_t j = 0; j < column; ++j) {
            mat << matrix[i][j] * val;
        }
    }
    return mat;
}
template<size_t row, size_t column>
Mat<row, column> Mat<row, column>::operator/(double val) {
    ASSERT(val != 0, "divide by zero error!");
    Mat<row, column> mat;
    for(size_t i = 0; i < row; ++i) {
        for(size_t j = 0; j < column; ++j) {
            mat << array_[i][j] / val;
        }
    }
    return mat;
}

template<size_t row, size_t column>
std::ostream& operator<<(std::ostream& os, Mat<row, column> mat) {
    for(size_t i = 0; i < row; ++i) {
        for(size_t j = 0; j < column; ++j) {
            os << mat[i][j];
            if(j != column - 1) { os << "\\"; }
        }
        os << " ";
    }
    // os << "\n";
    return os;
}


// template<size_t row, size_t column>
// std::ostream& operator<<(std::ostream& os, Mat<row, column> mat) {
//     for(size_t i = 0; i < row; ++i) {
//         for(size_t j = 0; j < column; ++j) {
//             os << mat[i][j];
//             if(j != column - 1) { os << " "; }
//         }
//         os << "\n";
//     }
//     return os;
// }


using Mat3 = Mat<3, 3>;
using Mat4 = Mat<4, 4>;


// Mat4 modelMatrix() {
//     Mat4 mat;
//     mat << 
// }
}

#endif