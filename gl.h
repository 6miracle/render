// #ifndef __RENDER_GL_H__
// #define __RENDER_GL_H__
// #include "maths/maths.hpp"
// #include "maths/Matrix.hpp"
// #include "tgaimage.h"
// #include "pch.h"
// #include <fstream>
// namespace render {

// extern Mat4 modelmat;
// extern Mat4 viewmat;
// extern Mat4 viewportmat;
// extern Mat4 projectionmat;


// // void modelmatrix(Vec3 eye, Vec3 center, Vec3 up);
// void modelmatrix(double angle);
// void viewMatrix(Vec3 eye, Vec3 center, Vec3 up);
// void projectionMatrix(double eye_fov, double aspect_ratio, double  zNear, double zFar);
// void projectionMatrix(double val);
// void viewPortMatrix( int w, int h);


// class IShader {
// public:
//     virtual ~IShader();
//     virtual Vec4 vertex(int face, int nthvert) = 0;
//     virtual bool fragment(Vec3 vec, TGAColor& color) = 0;
// };

// void triangle(Vec4 *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer);
// }


// #endif