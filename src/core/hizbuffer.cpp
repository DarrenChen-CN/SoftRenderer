#include "hizbuffer.hpp"
#include <iostream>
#include "GLFW/glfw3.h"
HiZBuffer::HiZBuffer(){}

HiZBuffer::HiZBuffer(int width, int height):width(width), height(height) {
    Init();
}

HiZBuffer::~HiZBuffer() {
    delete z_pyramid;
}

void HiZBuffer::Init() {
    assert(width > 0 && height > 0);
    z_buffer = new QuadNode[width * height];
    z_pyramid = Build(0, 0, width - 1, height - 1);
    std::cout << "build HiZBuffer done." << std::endl;
    Clear();
}

QuadNode *HiZBuffer::Build(int left, int down, int right, int up) {
    if(left > right || down > up)return nullptr; // 没有像素
    if(left == right && down == up)return &(z_buffer[(height - up - 1) * width + left]); // 单像素
    // 多像素
    QuadNode *res = new QuadNode(left, down, right, up);
    int middle_width = (left + right) / 2, middle_height = (up + down) / 2;
    // 分割成四个区域
    // 左上
    auto t = Build(left, middle_height + 1, middle_width, up);
    if(t != nullptr){
        t -> father = res;
        res -> children[res -> child_count++] = t;
    }
    // 左下
    t = Build(left, down, middle_width, middle_height);
    if(t != nullptr){
        t -> father = res;
        res -> children[res -> child_count++] = t;
    }
    // 右上
    t = Build(middle_width + 1,  middle_height + 1, right, up);
    if(t != nullptr){
        t -> father = res;
        res -> children[res -> child_count++] = t;
    }
    // 右下
    t = Build(middle_width + 1,  down, right, middle_height);
    if(t != nullptr){
        t -> father = res;
        res -> children[res -> child_count++] = t;
    }
    return res;
}

bool HiZBuffer::Judge(QuadNode* node, Bounds2 triangle_bound) {
    assert(node != nullptr);
    // if(node -> z != -1)
    //     std::cout << node -> z << " " << triangle_bound.z << std::endl;
    // 如果node的最远深度都比triangle_bound的最近深度大 说明triangle被遮挡
    if(node -> z >= triangle_bound.z)return true; // 被遮挡
    return false;
}

bool HiZBuffer::Contain(QuadNode* node, Bounds2 triangle_bound) {
    assert(node != nullptr);
    return node->left <= triangle_bound.left && node ->right >= triangle_bound.right && node -> down <= triangle_bound.down && node -> up >= triangle_bound.up;
}

bool HiZBuffer::Reject(Bounds2 triangle_bound) {
    // 从根节点开始 判断是否拒绝 指导找不到包含三角形的节点
    assert(z_pyramid != nullptr);
    QuadNode *node = z_pyramid;
    // std::cout << triangle_bound.left << " " << triangle_bound.right << " " << triangle_bound.down << " " << triangle_bound.up << " " << triangle_bound.z << std::endl;

    while(true){
        // 判断当前节点是否能够证明三角形被遮挡
        bool flag = Judge(node, triangle_bound);
        // if(node -> z != -1)
        // std::cout << node -> left << " " << node -> right <<" " << node -> down << " " << node -> up << " " << node -> z << std::endl;
        // 如果能够拒绝直接返回
        if(flag){
            // std::cout << triangle_bound.left << " "  << triangle_bound.right << " "  << triangle_bound.down << " "  << triangle_bound.up << " " << std::endl;
            // std::cout << node -> z << " " << triangle_bound.z << std::endl;
            // std::cout << std::endl;
            return true;
        }
        // 不能够拒绝 找到下一个包含三角形的子节点
        QuadNode* next_node = nullptr;
        for(int i = 0; i < node -> child_count; i ++){
            auto t = node -> children[i];
            if(Contain(t, triangle_bound)){
                next_node = t;
                break;
            }
        }
        // 如果没找到 对三角形进行光栅化
        if(next_node == nullptr){
            // std::cout << std::endl;
            return false;
        }
        // 如果找到 继续循环
        node = next_node;
    }
    // std::cout << 111 << std::endl;
    return false;
}

void HiZBuffer::Update(QuadNode *node, float z) {
    assert(node != nullptr);
    auto t = node;
    while(t){
        // 更新当前节点
        float this_z = t -> z;
        t -> z = z;
        // 判断是否需要更新父节点
        auto fa = t -> father;
        if(fa == nullptr)return ; // 没有父节点
        if(fabs(fa -> z - this_z) > eps)return; // 父节点的最小值不是来自当前节点
        // 如果当前的最小值就是父节点的最小值 那么有可能需要更新
        // bool need_update = false;
        float min_z = 0;
        // 寻找父节点的所有孩子的最小深度
        for(int i = 0; i < fa -> child_count; i ++){
            min_z = std::min(fa -> children[i] -> z, min_z);
        }
        // if(min_z != -1)
        // std::cout << z << " " << min_z << std::endl;
        if(min_z > fa -> z){
            // 需要更新
            t = fa;
            z = min_z;
        }else return ;
    }
    return ;
}

bool HiZBuffer::WirteZBuffer(int x, int y, float z) {
    if(x < 0 || y < 0 || x >= width || y >= height)return false;
    y = height - y - 1;
    int pos = y * width + x;
    // std::cout << x << " " << y << " " << pos << std::endl;
    if(z_buffer[pos].z > z)return false; // 不需要修改
    // 需要修改zbuffer 同时要更新四叉树
    Update(&z_buffer[pos], z);
    // std::cout << z_pyramid->z << std::endl;
    // std::cout << z_buffer[pos].z << std::endl;
    return true;
}

void HiZBuffer::Clear() {
    // check();
   Clear(z_pyramid);
}

void HiZBuffer::Clear(QuadNode *node) {
    if(node == nullptr)return ;
    // if(node -> z != -1)std::cout << node -> z << std::endl;
    node -> z = -1;
    for(int i = 0; i < node -> child_count; i ++)Clear(node -> children[i]);
}
