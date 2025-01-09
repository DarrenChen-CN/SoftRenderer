#pragma once 
#include "define.hpp"

// 四叉树节点
class QuadNode{
public:
    QuadNode(){}
    QuadNode(int left, int dwon, int right, int up): left(left), down(down), right(right), up(up){
        child_count = 0;
        z = -1;
    }

    ~QuadNode(){
        for(int i = 0; i < child_count; i ++){
            if(children[i] != nullptr)delete children[i];
        }
    }

    int left, right, up, down;
    int child_count; // 子节点个数
    QuadNode* children[4] = {nullptr};
    QuadNode* father = nullptr;
    float z; // 区域里深度的最小值

};