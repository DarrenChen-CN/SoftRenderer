#pragma once 
#include "shader.hpp"
#include "trianglemesh.hpp"

class Triangle{
public:
    Triangle(int id, unsigned int p0, unsigned int p1, unsigned int p2, TriangleMesh *meshes_): id(id), p0(p0), p1(p1), p2(p2){
        assert(meshes_ != nullptr);
        this -> meshes = meshes_;
    }

    Triangle(TriangleMesh *meshes_, int id){
        assert(meshes_ != nullptr);
        this -> meshes = meshes_;
        auto face_num = meshes -> GetFaceNum();
        assert(id < face_num);
        auto indices = meshes -> GetIndices();
        p0 = indices[3 * id + 0];
        p1 = indices[3 * id + 1]; 
        p2 = indices[3 * id + 2];
    }
    Triangle(){};

    Vertex GetVertex(int idx){
        assert(meshes != nullptr);
        assert(idx >= 0 && idx <= 2);
        if(idx == 0)return meshes -> GetVertex(p0);
        else if(idx == 1)return meshes -> GetVertex(p1);
        return meshes -> GetVertex(p2);
    }

    int GetID(){return id;}

    unsigned int GetIndex(int idx){
        assert(meshes != nullptr);
        assert(idx >= 0 && idx <= 2);
        if(idx == 0)return p0;
        else if(idx == 1)return p1;
        return p2;
    }

    int left, right, up, down;
private:
    TriangleMesh *meshes = nullptr;
    int id;
    unsigned int p0, p1, p2;
};