#pragma once
#include "define.hpp"
#include <vector>
#include "shader.hpp"
#include <algorithm>
#include <iostream>
// #include "framebuffer.hpp"
class FrameBuffer;

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
    ClassifiedEdge(float x, float dx, int dy, int id, int y_max, VertexShaderOutput &v0, VertexShaderOutput &v1): x(x), dx(dx), dy(dy), id(id), y_max(y_max){
        vs_output[0] = v0, vs_output[1] = v1;
    }
    ClassifiedEdge(){}

    float x; // 边上端点的x坐标
    float dx; // 相邻两条扫描线交点的x坐标差
    int dy; // 边跨越的扫描线数目
    int id; // 边所属的的三角形编号
    int y_max; // 上端点坐标
    VertexShaderOutput vs_output[2]; // 上下端点参数
};

// 活化多边形类
class ActiveTriangle{
public:
    ActiveTriangle(int id, int dy): id(id), dy(dy){}
    ActiveTriangle(){}

    int id; // 三角形编号
    int dy; // 多边形跨越的 剩余 扫描线数目
    // int y; // 交点y坐标

};

// 活化边类
class ActiveEdge{
public:
    ActiveEdge(float xl, float xr, float dxl, float dxr, int dyl, int dyr, float zl, float dzx, float dzy, int id):
    xl(xl), xr(xr), dxl(dxl), dxr(dxr), dyl(dyl), dyr(dyr), zl(zl), dzx(dzx), dzy(dzy), id(id){}
    ActiveEdge(){}

    float xl, xr; // 左右交点的x坐标 当前
    float dxl, dxr; // 左右交点边上两相邻扫描线交点的x坐标之差
    int dyl, dyr; // 以和左右交点所在边相交的扫描线数为初值 以后向下每处理一条扫描线减1
    float zl; // 左脚点深度值
    float dzx, dzy; // 沿扫描线向右（下）走过一个像素时，多边形所在平面的深度增量。
    int id; // 交点对所在的多边形编号

    VertexShaderOutput l_vs_output[2], r_vs_output[2]; // 左右边的顶点信息
};

// 记录每个面片的信息
class ScanLineTriangleInfo{
public:
    ScanLineTriangleInfo(){}
    ScanLineTriangleInfo(int id, VertexShaderOutput &v0, VertexShaderOutput &v1, VertexShaderOutput &v2, Shader *shader): id(id){
        vs_output[0] = v0, vs_output[1] = v1, vs_output[2] = v2;
        this -> shader = shader;
        x_min = std::min(v0.screen_position(0), std::min(v1.screen_position(0), v2.screen_position(0)));
        x_max = std::max(v0.screen_position(0), std::max(v1.screen_position(0), v2.screen_position(0)));
        y_min = std::min(v0.screen_position(1), std::min(v1.screen_position(1), v2.screen_position(1)));
        y_max = std::max(v0.screen_position(1), std::max(v1.screen_position(1), v2.screen_position(1)));
        dy = y_max - y_min;
        // 按y坐标升序排序
        std::sort(vs_output, vs_output + 3, [](const VertexShaderOutput &a, const VertexShaderOutput &b){
            if(a.screen_position(1) != b.screen_position(1))
                return a.screen_position(1) < b.screen_position(1);
            // y相等按照x从小到大排序
            return a.screen_position(0) < b.screen_position(0);
        });
    }
    int id; // 三角形编号
    VertexShaderOutput vs_output[3];
    int x_min, y_min, x_max, y_max;
    int dy;

    // 获取三条边的分类边数组
    std::vector<ClassifiedEdge> GetEdges(){
        std::vector<ClassifiedEdge> res;
        // 三条边
        float x, dx;
        int dy;
        // 1 (v1 -> v0)
        x = vs_output[1].screen_position(0);
        dx = 1.f * (vs_output[0].screen_position(0) - vs_output[1].screen_position(0)) / (vs_output[1].screen_position(1) - vs_output[0].screen_position(1));
        dy = ((int)vs_output[1].screen_position(1) - (int)vs_output[0].screen_position(1));
        // std::cout << vs_output[1].screen_position(1) << " " << vs_output[0].screen_position(1) << " " << dy << std::endl;
        res.push_back({x, dx, dy, id, vs_output[1].screen_position(1), vs_output[1], vs_output[0]});
        // 2（v2 -> v0）
        x = vs_output[2].screen_position(0);
        dx = 1.f * (vs_output[0].screen_position(0) - vs_output[2].screen_position(0)) / (vs_output[2].screen_position(1) - vs_output[0].screen_position(1));
        dy = ((int)vs_output[2].screen_position(1) - (int)vs_output[0].screen_position(1));;
        res.push_back({x, dx, dy, id, vs_output[2].screen_position(1), vs_output[2], vs_output[0]});
        // 3（v2 -> v1）
        x = vs_output[2].screen_position(0);
        dx = 1.f * (vs_output[1].screen_position(0) - vs_output[2].screen_position(0)) / (vs_output[2].screen_position(1) - vs_output[1].screen_position(1));
        dy = ((int)vs_output[2].screen_position(1) - (int)vs_output[1].screen_position(1));;
        res.push_back({x, dx, dy, id, vs_output[2].screen_position(1), vs_output[2], vs_output[1]});
        return res;
    }


    Shader *shader = nullptr;
};


class ScanLineZBuffer{
public:
    ScanLineZBuffer();
    ScanLineZBuffer(int width, int height);
    ~ScanLineZBuffer();
    void Init(int id, std::vector<ScanLineTriangleInfo> &tris);
    void Scan(unsigned char *framebuffer);
    bool WriteZBuffer(int x, int y, float depth);
    int GetSceneID();

private:
    ActiveEdge GetActiveEdge(ClassifiedEdge e1, ClassifiedEdge e2); // 根据两条边构造出活化边
    void UpdateActiveEdge(ActiveEdge &e, ClassifiedEdge new_e); // 当有一条边的dy小于0时，找到新边并更新

    int width, height;
    float *z_buffer = nullptr;
    int scene_id = -1; // 场景id
    
    std::vector<std::vector<ClassifiedTriangle>> classified_triangle_table; // 分类多边形表
    std::vector<std::vector<ClassifiedEdge>> classified_edge_table; // 分类边表
    std::vector<ActiveTriangle> active_triangle_table; // 活化多边形表
    std::vector<ActiveEdge> active_edge_table; // 活化边表
    // std::vector<std::vector<ActiveEdge>> active_edge_list; // 每一个三角形的两个活化边 在初始化时提前计算

    std::vector<ScanLineTriangleInfo> tris;
};