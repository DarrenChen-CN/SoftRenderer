#include "mathutil.hpp"
#include <iostream>
Vec4f Vec3ToVec4(Vec3f v){
    Vec4f res;
    res(0) = v(0);
    res(1) = v(1);
    res(2) = v(2);
    res(3) = 1.0;
    return res; 
}

Vec4f PerspectiveDivision(Vec4f v){
    assert(fabs(v(3) - 0.f) > eps);
    Vec4f res;
    res(0) = v(0) / v(3);
    res(1) = v(1) / v(3);
    res(2) = v(2) / v(3);
    res(3) = 1.f;
    return res;
}

Mat4f GetViewPortMatrix(int ox, int oy, int width, int height){
    Mat4f res = Mat4f::Identity();
    res(0, 0) = width / 2.f;
    res(0, 3) = ox + (width / 2.f);
    res(1, 1) = height / 2.f;
    res(1, 3) = oy + (height / 2.f);
    // std::cout << res << std::endl;
    return res;
}

float AngleToRadian(float angle){
    return angle / 180 * PI;
}

Mat4f Translate(Vec3f v)
{
    Mat4f res = Mat4f::Identity();
    res(0, 3) = v(0);
    res(1, 3) = v(1);
    res(2, 3) = v(2);
    // std::cout << res << std::endl;
    return res;
}

Mat4f Scale(Vec3f v) {
    Mat4f res = Mat4f::Identity();
    res(0, 0) = v(0);
    res(1, 1) = v(1);
    res(2, 2) = v(2);
    return res;
}

Mat4f RotateX(Vec3f v, float theta) {
    Mat4f res = Mat4f::Identity();
    theta = AngleToRadian(theta);
    float cos_theta = cos(theta), sin_theta = sin(theta);
    res(1, 1) = cos_theta;
    res(1, 2) = -sin_theta;
    res(2, 1) = sin_theta;
    res(2, 2) = cos_theta;
    return res;
}

Mat4f RotateY(Vec3f v, float theta) {
    Mat4f res = Mat4f::Identity();
    theta = AngleToRadian(theta);
    float cos_theta = cos(theta), sin_theta = sin(theta);
    res(0, 0) = cos_theta;
    res(0, 2) = sin_theta;
    res(2, 0) = -sin_theta;
    res(2, 2) = cos_theta;
    return res;
}

Mat4f RotateZ(Vec3f v, float theta) {
    Mat4f res = Mat4f::Identity();
    theta = AngleToRadian(theta);
    float cos_theta = cos(theta), sin_theta = sin(theta);
    res(0, 0) = cos_theta;
    res(0, 1) = -sin_theta;
    res(1, 0) = sin_theta;
    res(1, 1) = cos_theta;
    return res;
}

// lookat为注视点
Mat4f GetViewMatrix(Vec3f pos, Vec3f lookat, Vec3f up) {
    Mat4f res = Mat4f::Identity();
    Mat4f R = Mat4f::Identity(), T = Mat4f::Identity();

    T = Translate(-pos);
    
    Vec3f g = (lookat - pos).normalized();
    Vec3f e = g.cross(up);
    Vec3f t = e.cross(g);
    R(0, 0) = e(0), R(0, 1) = e(1), R(0, 2) = e(2);
    R(1, 0) = t(0), R(1, 1) = t(1), R(1, 2) = t(2);
    R(2, 0) = -g(0), R(2, 1) = -g(1), R(2, 2) = -g(2);

    res = R * T;
    return res;
}

Mat4f GetOrthoMatrix(float l, float r, float b, float t, float f, float n) {
    Mat4f res = Mat4f::Identity();

    Mat4f T = Translate(Vec3f(-(l + r) / 2, -(b + t) / 2, -(n + f) / 2));
    Mat4f S = Scale(Vec3f(2 / (r - l), 2 / (t - b), 2 / (n - f)));

    res = S * T;
    return res;
}

// aspect为宽高比
Mat4f GetPerspMatrix(float fovy, float aspect, float z_near, float z_far) {
    Mat4f res = Mat4f::Identity();

    fovy = AngleToRadian(fovy);
    // z为负值 故此处计算的是bottom
    float bottom = z_near * tan(fovy / 2), top = -bottom;
    float right = top * aspect, left = -right;

    Mat4f ortho = GetOrthoMatrix(left, right, bottom, top, z_far, z_near);
    Mat4f persp_to_ortho = Mat4f::Zero();
    persp_to_ortho(0, 0) = z_near;
    persp_to_ortho(1, 1) = z_near;
    persp_to_ortho(2, 2) = z_near + z_far;
    persp_to_ortho(2, 3) = -z_far * z_near;
    persp_to_ortho(3, 2) = 1;

    res = ortho * persp_to_ortho;
    return res;
}

Vec3f Max(const Vec3f &v1, const Vec3f &v2) {
    Vec3f res;
    for(int i = 0; i < 3; i ++){
        res(i) = std::max(v1(i), v2(i));
    }
    return res;
}

Vec3f Min(const Vec3f &v1, const Vec3f &v2) {
    Vec3f res;
    for(int i = 0; i < 3; i ++){
        res(i) = std::min(v1(i), v2(i));
    }
    return res;
}

