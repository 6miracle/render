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
    Mat(const std::initializer_list<double>& lists);
    // 转置
    Mat<column, row> transpose();
    Mat<row, column> invert(); 

    size_t columns()  { return column; }
    size_t rows() { return rows; }

    Mat<row, column>& operator<<(double val);
    Mat<row, column>& operator<<(Vec<column> vec);
    Mat<row, column>& operator,(double val);
    // template<typename ...Args>
    // Mat<row, column>& operator<<(Args... args);
    Mat<row, column> operator*(double val);
    Vec<row> operator*(Vec<column> vec);
    Mat<row, column> operator/(double val);

    void clear();

    // 运算
    template<size_t num> 
    Mat<row, num> operator*(const Mat<column, num>& mat) {
        Mat<row, num> res;
        for(size_t i = 0; i < row; ++i) {
            double result = 0;
            for(size_t j = 0; j < num; ++j) {
                for(size_t t = 0; t < column; ++t) {
                    result += array_[i][t] * mat[t][j]; 
                }
                res << result;
                result = 0;
            }
        }
        return res;
    }

    const double* operator[] (int i) const {
        ASSERT(i < N, "访问越界");
        return array_[i];
    }
    double* operator[](int i) {
        ASSERT(i < N, "访问越界");
        return array_[i];
    }
private:
    double array_[row][column] = {0};
    size_t nums_ = 0;
};

template<size_t row, size_t column> 
Mat<row, column>::Mat(const std::initializer_list<double>& lists) {
    for(auto& val : lists) {
        array_[nums_ / column][nums_ % column] = val;
        ++nums_;
    }
}

// 转置
template<size_t row, size_t column> 
Mat<column, row> Mat<row, column>::transpose() {
    Mat<column, row> mat;
    for(size_t i = 0; i < column; ++i) {
        for(size_t j = 0; j < row; ++j) {
            mat << array_[j][i];
        }

    }
    return mat;
}

// 求逆
template<size_t row, size_t column> 
Mat<row, column> Mat<row, column>::invert()  {
    ASSERT(row == column, " row must equal to column");
    Mat<column, column> l, u;
    l[0][0] = l[1][1] = l[2][2] = l[3][3] = 1;
    for(size_t k = 0; k < row; ++k) {
        //  std::cout << " k =" << k <<"\n";
        // 求u
        for(size_t j = k; j < row; ++j) {
            u[k][j] = array_[k][j];
            for(size_t i = 0; i < k; ++i) {
                u[k][j] -= l[k][i] * u[i][j];
            }
        }
        // std::cout << " k =" << k <<"\n";
        // 求l
        for(size_t j = k + 1; j < row; ++j) {
            l[j][k] = array_[j][k];
            for(size_t i = 0; i < k; ++i) {
                l[j][k] -= l[j][i] * u[i][k];
            }
            ASSERT(u[k][k] != 0, "sa");
            l[j][k] /= u[k][k];
        }
    }
    
    // 求l,u的逆
    Mat<column, column> ll, uu;

    for(size_t i = 0; i < row; ++i) {
        for(size_t j = 0; j < row; ++j) {
            if(i == j) { ll << 1; }
            else if(i < j) { ll << 0; }
            else if(i > j) {
                 double tmp = 0;
                for(size_t k = j; k <= i - 1; ++k) {
                    tmp += l[i][k] * ll[k][j];
                }
                ll << -tmp; 
            }
        }
    }
    for(int j = 0; j < row ; ++j) {
        for(int i = row - 1; i >= 0; --i) {
            if(i == j) { uu[i][j] =  1.0 / u[i][j];  }
            else if(i < j) {
                double tmp = 0;
                for(size_t k = i + 1; k <= j; ++k) {
                    tmp += u[i][k] * uu[k][j];
                }
                uu[i][j] =  tmp / (-u[i][i]);
            } else if(i > j) { uu[i][j] = 0; }
        }
    }

    return uu * ll;
}
template<size_t row, size_t column>
std::ostream& operator<<(std::ostream& os, Mat<row, column> mat) {
    for(size_t i = 0; i < row; ++i) {
        for(size_t j = 0; j < column; ++j) {
            os << mat[i][j] << " ";
            // if(j != column - 1) { os << ""; }
        }
        os << "\n";
    }
    // os << "\n";
    return os;
}
template<size_t row, size_t column>
Mat<row, column>& Mat<row, column>::operator<<(double val) {
    if(nums_ < column * row) {
        array_[nums_ / column][nums_ % column] = val;
        ++nums_;
    }
    return *this;
}


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
Vec<row> Mat<row, column>::operator*(Vec<column> vec) {
    Vec<row> res;
    for(size_t i = 0; i < row; ++i) {
        for(size_t j = 0; j < column; ++j) {
            res[i] += array_[i][j] * vec[j];
        }
    }
    return res;
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
void Mat<row, column>::clear() {
    nums_ = 0;
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

}

#endif