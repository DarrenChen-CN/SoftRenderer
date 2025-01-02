#pragma once 
#include "trianglemesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

class Model{
public:
    Model(std::string path);
    std::vector<TriangleMesh> GetMeshes();
private:
    std::vector<TriangleMesh> meshes;
    std::string directory;

    void LoadModel(std::string path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    TriangleMesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

};