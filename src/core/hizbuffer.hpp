#pragma once 
#include "quadnode.hpp"
#include "bounds.hpp"

class HiZBuffer{
public:
    HiZBuffer();
    HiZBuffer(int width, int height);
    ~HiZBuffer();

    void Init();
    QuadNode *Build(int left, int down, int right, int up); //构建四叉树
    bool Judge(QuadNode* node, Bounds2 triangle_bound); // 判断是否拒绝（当前节点）
    bool Contain(QuadNode* node, Bounds2 triangle_bound); // 判断节点是否包含三角形
    bool Reject(Bounds2 triangle_bound); // 判断hi-z能否直接拒绝三角形（若返回true则不进行光栅化）
    bool WirteZBuffer(int x, int y, float z); // 光栅化时修改zbuffer并更新四叉树
    void Update(QuadNode *node, float z); // 更新四叉树
    void Clear(); // 重置深度值
    void Clear(QuadNode *node);
private:
    int width, height;
    QuadNode* z_buffer = nullptr;
    QuadNode* z_pyramid = nullptr;

};