#pragma once 
#include "define.hpp"
#include "mathutil.hpp"
// 二维三角形的包围盒
class Bounds2{
public:
    Bounds2(){}
    Bounds2(Vec4f v0, Vec4f v1, Vec4f v2): v0(v0), v1(v1), v2(v2){
        left = std::min(v0(0), std::min(v1(0), v2(0)));
        right = std::max(v0(0), std::max(v1(0), v2(0)));
        down = std::min(v0(1), std::min(v1(1), v2(1)));
        up = std::max(v0(1), std::max(v1(1), v2(1)));
        z = std::max(v0(2), std::max(v1(2), v2(2)));
    }

    Vec4f v0, v1, v2; // 三角形的屏幕空间坐标
    float z; // 三个顶点的最大z值 最近深度
    int left, right, up, down;
};

// 空间包围盒
class Bounds3{
public:
    Bounds3(){

    }
    Bounds3(Vec4f v0, Vec4f v1, Vec4f v2){
        p_min(0) = std::min(v0(0), std::min(v1(0), v2(0)));
        p_max(0) = std::max(v0(0), std::max(v1(0), v2(0)));
        p_min(1) = std::min(v0(1), std::min(v1(1), v2(1)));
        p_max(1) = std::max(v0(1), std::max(v1(1), v2(1)));
        p_max(2) = std::max(v0(2), std::max(v1(2), v2(2)));
        p_min(2) = std::min(v0(2), std::min(v1(2), v2(2)));

        center = (p_min + p_max) / 2;

        min_z = p_min(2);
        max_z = p_max(2);
    }

    Bounds3(Vec3f p_min, Vec3f p_max): p_min(p_min), p_max(p_max){
        center = (p_min + p_max) / 2;
        min_z = p_min(2);
        max_z = p_max(2);
    }

    
    Vec3f p_min = {FLOATMAX, FLOATMAX, FLOATMAX}, p_max = {-FLOATMAX, -FLOATMAX, -FLOATMAX};
    float min_z, max_z;
    Vec3f center; // 中点

    static Bounds3 Union(const Bounds3 &a, const Bounds3 &b){
        Vec3f p_min = Min(a.p_min, b.p_min);
        Vec3f p_max = Max(a.p_max, b.p_max);
        return Bounds3(p_min, p_max);
    }
};