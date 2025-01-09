#pragma once 
#include "trianglemesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "shader.hpp"

class Model{
public:
    Model(std::string path);
    std::vector<TriangleMesh> GetMeshes();
    void SetShader(Shader *shader);
    Shader *GetShader();
    int GetFaceNum();
    int GetMaxVertexNum();
    int GetTotalVertexNum();

private:
    std::vector<TriangleMesh> meshes;
    std::string directory;
    Shader *shader = nullptr;
    int face_num = 0, max_vertex_num = 0; // 面片个数以及mesh中顶点最多的数量
    int total_vertex_num = 0;
    void LoadModel(std::string path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    TriangleMesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

};