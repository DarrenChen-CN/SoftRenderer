#include "trianglemesh.hpp"

TriangleMesh::TriangleMesh(){}

TriangleMesh::TriangleMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices){
    this -> vertices = vertices;
    this -> indices = indices;
    face_num = indices.size() / 3;
}

TriangleMesh::~TriangleMesh() {
}

void TriangleMesh::SetShader(Shader *shader) {
    assert(shader != nullptr);
    this -> shader = shader;
}

void TriangleMesh::SetVertices(std::vector<Vertex> vertices) {
    this -> vertices = vertices;
}

std::vector<Vertex> TriangleMesh::GetVertices() {
    return vertices;
}

std::vector<unsigned int> TriangleMesh::GetIndices() {
    return indices;
    face_num = indices.size() / 3;
}

int TriangleMesh::GetVertexNum() {
    return vertices.size();
}

Vertex TriangleMesh::GetVertex(int idx) {
    assert(idx < GetVertexNum());
    return vertices[idx];
}

Shader *TriangleMesh::GetShader() {
    assert(shader != nullptr);
    return shader;
}

int TriangleMesh::GetFaceNum() {
    return face_num;
}