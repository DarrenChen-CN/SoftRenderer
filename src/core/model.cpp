#include "model.hpp"
#include <iostream>
#include <cstring>

Model::Model(std::string path) {
    LoadModel(path);
    std::cout << "Load Model: " << path << std::endl;
    for(auto& mesh: meshes){
        auto vertex_num = mesh.GetVertexNum();
        max_vertex_num = std::max(max_vertex_num, vertex_num);
        total_vertex_num += vertex_num;
        face_num += mesh.GetFaceNum();
    }
}

TriangleMesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
        Vertex vertex;
        // 处理顶点位置、法线和纹理坐标
        Vec3f vector;
        // 顶点
        vector(0) = mesh->mVertices[i].x;
        vector(1) = mesh->mVertices[i].y;
        vector(2) = mesh->mVertices[i].z; 
        vertex.position = vector;
        // 法线
        vector(0) = mesh->mNormals[i].x;
        vector(1) = mesh->mNormals[i].y;
        vector(2) = mesh->mNormals[i].z;
        vertex.normal = vector;
        // 纹理
        if(mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
        {
            Vec2f vec;
            vec(0) = mesh->mTextureCoords[0][i].x; 
            vec(1) = mesh->mTextureCoords[0][i].y;
            vertex.texture_coord = vec;
        }
        else
            vertex.texture_coord = Vec2f(0.0f, 0.0f);
        vertices.push_back(vertex);
    }
    // 处理索引
    for(unsigned int i = 0; i < mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // // 处理材质
    // if(mesh->mMaterialIndex >= 0){
    //     aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    //     std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, 
    //                                         aiTextureType_DIFFUSE, "texture_diffuse");
    //     textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //     std::vector<Texture> specularMaps = LoadMaterialTextures(material, 
    //                                         aiTextureType_SPECULAR, "texture_specular");
    //     textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // }

    return TriangleMesh(vertices, indices);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene) {
    // 处理节点所有的网格（如果有的话）
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(ProcessMesh(mesh, scene));         
    }
    // 接下来对它的子节点重复这一过程
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Model::LoadModel(std::string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);    

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);

    // std::cout << "Load model done." << std::endl;
}

std::vector<TriangleMesh> Model::GetMeshes() {
    return meshes;
}

void Model::SetShader(Shader *shader) {
    assert(shader != nullptr);
    this -> shader = shader;
    // for(auto mesh: meshes){
    //     mesh.SetShader(shader);
    // }
}

Shader *Model::GetShader() {
    assert(shader != nullptr);
    return shader;
}

int Model::GetFaceNum() {
    return face_num;
}

int Model::GetMaxVertexNum() {
    return max_vertex_num;
}

int Model::GetTotalVertexNum() {
    return total_vertex_num;
}