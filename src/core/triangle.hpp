#pragma once 
#include "shader.hpp"
#include "trianglemesh.hpp"

class Triangle{
public:
    Triangle(unsigned int p0, unsigned int p1, unsigned int p2, TriangleMesh *meshes_): p0(p0), p1(p1), p2(p2){
        assert(meshes_ != nullptr);
        this -> meshes = meshes_;
    }
    Triangle(){};

    Vertex GetVertex(int idx){
        assert(meshes != nullptr);
        assert(idx >= 0 && idx <= 2);
        if(idx == 0)return meshes -> GetVertex(p0);
        else if(idx == 1)return meshes -> GetVertex(p1);
        return meshes -> GetVertex(p2);
    }
private:
    TriangleMesh *meshes = nullptr;
    unsigned int p0, p1, p2;
};