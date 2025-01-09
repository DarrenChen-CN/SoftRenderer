#pragma once
#include "define.hpp"

// 分类多边形（只支持三角形）
class ClassifiedTriangle{
public:
    ClassifiedTriangle(int id, int dy, int y_max): id(id), dy(dy), y_max(y_max){}
    ClassifiedTriangle(){}

    int id; // 三角形编号
    int dy; // 三角形跨越的扫描线数目
    int y_max; // 在屏幕空间的y的最大值
};

// 分类边类
class ClassifiedEdge{
public:
    ClassifiedEdge(float x, float dx, int dy, int id, int y_max): x(x), dx(dx), dy(dy), id(id), y_max(y_max){}
    ClassifiedEdge(){}

    float x; // 边上端点的x坐标
    float dx; // 相邻两条扫描线交点的x坐标差
    int dy; // 边跨越的扫描线数目
    int id; // 边所属的的三角形编号
    int y_max; // 上端点坐标
};

// 活化多边形类
class ActiveTriangle{
public:
    ActiveTriangle(int id, int dy, int y): id(id), dy(dy), y(y){}
    ActiveTriangle(){}

    int id; // 三角形编号
    int dy; // 多边形跨越的 剩余 扫描线数目
    int y; // 交点y坐标

};

// 活化边类
class ActiveEdge{
public:
    ActiveEdge(float xl, float xr, float dxl, float dxr, int dyl, int dyr, float zl, float dzx, float dzy, int id):
    xl(xl), xr(xr), dxl(dxl), dxr(dxr), dyl(dyl), dyr(dyr), zl(zl), dzx(dzx), dzy(dzy), id(id){}
    ActiveEdge(){}

    float xl, xr; // 左右交点的x坐标
    float dxl, dxr; // 左右交点边上两相邻扫描线交点的x坐标之差
    int dyl, dyr; // 以和左右交点所在边相交的扫描线数为初值 以后向下每处理一条扫描线减1
    float zl; // 左脚点深度值
    float dzx, dzy; // 沿扫描线向右（下）走过一个像素时，多边形所在平面的深度增量。
    int id; // 交点对所在的多边形编号
};

class ScanLineZBuffer{
public:

private:
    int width, height;
    float *zBuffer;
    

};