#pragma once 
#include "define.hpp"
#include "bounds.hpp"
#include "shader.hpp"
#include <algorithm>
#include <iostream>

// 记录每个面片的信息
class BVHTriangleInfo{
public:
    BVHTriangleInfo(){}
    BVHTriangleInfo(VertexShaderOutput &v0, VertexShaderOutput &v1, VertexShaderOutput &v2, Shader *shader){
        vs_output[0] = v0, vs_output[1] = v1, vs_output[2] = v2;
        bounds = {v0.world_position, v1.world_position, v2.world_position};
        this -> shader = shader;
    }

    // int id; // 面片的位置
    Bounds3 bounds; // 三角形包围盒
    VertexShaderOutput vs_output[3];
    Shader *shader = nullptr;
};

// BVH节点
class BVHNode{
public:
    BVHNode(){};

    // 叶节点
    void InitLeaf(int offset, int n, Bounds3 b){
        triangle_offset = offset;
        n_triangles = n;
        bounds = b;
        children[0] = children[1] = nullptr;
    }

    // 非叶节点
    void InitInterior(BVHNode* c0, BVHNode *c1){
        assert(c0 != nullptr && c1 != nullptr);
        children[0] = c0, children[1] = c1;
        bounds = Bounds3::Union(c0 -> bounds, c1 -> bounds);
        n_triangles = 0; // 以此来判断是否为非叶节点
    }

    Bounds3 bounds;
    BVHNode *children[2];
    int n_triangles; // 节点内的三角形数量
    int triangle_offset; // 三角形在排序完的数组中的位置
};

class BVH{
public:
    BVH(int id, std::vector<BVHTriangleInfo> &tris){
        if(tris.size() == 0)return ;
        scene_id = id;

        // 构建BVH
        std::vector<BVHTriangleInfo> ordered_triangles; // 有序的结果
        root = RecursiveBuild(tris, 0, tris.size(), ordered_triangles);
        triangles.swap(ordered_triangles);

        // 记录叶节点用于遍历 深度优先遍历
        FindLeafNode(root);

        std::cout << "build BVH done." << std::endl;
    }

    ~BVH(){
        release(root);
    }

    std::vector<BVHNode *> GetLeafNode(){
        return leaf_nodes;
    }

    int GetID(){
        return scene_id;
    }

    std::vector<BVHTriangleInfo> GetTriangleInfo(){
        return triangles;
    }

private:
    std::vector<BVHTriangleInfo> triangles;
    std::vector<BVHNode *> leaf_nodes;
    BVHNode *root = nullptr;
    int scene_id; // 场景的id

    BVHNode *RecursiveBuild(std::vector<BVHTriangleInfo> &triangle_info, int start, int end, std::vector<BVHTriangleInfo> &ordered_prims){
        BVHNode* node = new BVHNode();
        // 计算所有元素包围盒
        Bounds3 bounds;
        for(int i = start; i < end; i ++){
            bounds = Bounds3::Union(bounds, triangle_info[i].bounds);
        }
        int n_triangles = end - start;
        if(n_triangles == 1){
            // 只有一个三角形
            int triangle_offset = ordered_prims.size(); 
            ordered_prims.push_back(triangle_info[start]);
            node -> InitLeaf(triangle_offset, n_triangles, bounds);
        }else{
            // 按照z轴分割 平分
            int mid = (start + end) / 2;
            std::nth_element(&triangle_info[start], &triangle_info[mid], &triangle_info[end - 1] + 1, [](const BVHTriangleInfo &a, const BVHTriangleInfo &b){
                return a.bounds.max_z > b.bounds.max_z;
            });// 对中间位置排序
            node -> InitInterior(RecursiveBuild(triangle_info, start, mid, ordered_prims), RecursiveBuild(triangle_info, mid, end, ordered_prims));
        }
        return node;
    }

    void FindLeafNode(BVHNode *root){
        if(root == nullptr)return;
        if(root -> n_triangles > 0){
            // 叶节点
            leaf_nodes.push_back(root);
        }else{
            // 非叶节点 深度优先遍历
            FindLeafNode(root -> children[0]);
            FindLeafNode(root -> children[1]);
        }
    }

    void release(BVHNode *root){
        if(root == nullptr)return;
        if(root -> n_triangles > 0){
            // 叶节点
            delete root;
        }else {
            release(root -> children[0]);
            release(root -> children[1]);
        }
    }
};