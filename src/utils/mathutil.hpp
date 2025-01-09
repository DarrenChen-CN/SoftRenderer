#pragma once 
#include <math.h>
#include "define.hpp"

float AngleToRadian(float angle);
Vec4f Vec3ToVec4(Vec3f v);
Vec4f PerspectiveDivision(Vec4f v);
Mat4f GetViewPortMatrix(int ox, int oy, int width, int height);
Mat4f Translate(Vec3f v);
Mat4f Scale(Vec3f v);
Mat4f RotateX(Vec3f v, float theta);
Mat4f RotateY(Vec3f v, float theta);
Mat4f RotateZ(Vec3f v, float theta);
Mat4f GetViewMatrix(Vec3f pos, Vec3f lookat, Vec3f up);
Mat4f GetOrthoMatrix(float l, float r, float b, float t, float f, float n);
Mat4f GetPerspMatrix(float fovy, float aspect, float z_near, float z_far);

Vec3f Min(const Vec3f &v1, const Vec3f &v2);
Vec3f Max(const Vec3f &v1, const Vec3f &v2);

