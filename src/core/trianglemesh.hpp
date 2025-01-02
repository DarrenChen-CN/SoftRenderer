#pragma once 
#include "vertex.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <vector>

class TriangleMesh{
public:
    TriangleMesh();
    ~TriangleMesh();
    TriangleMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void SetVertices(std::vector<Vertex> vertices);
    void SetShader(Shader *shader);
    Vertex GetVertex(int idx);
    int GetVertexNum();
    std::vector<Vertex> GetVertices();
    std::vector<unsigned int> GetIndices();
    Shader *GetShader();
    int GetFaceNum();


private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    int face_num = 0;
    Shader *shader = nullptr;
};