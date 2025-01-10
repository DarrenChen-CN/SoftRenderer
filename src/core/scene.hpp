#pragma once 
#include "model.hpp"

class Scene{
public:
    Scene();
    int AddModel(Model model); // 返回索引
    Model &GetModel(int id);
    std::vector<Model> GetModels();
    int GetModelNum();
    int GetMaxVertexNum();
    int GetFaceNum();
    int GetTotalVertexNum();
    int GetID();
    static int id;
private:
    int scene_id;
    std::vector<Model> models;
    int model_num = 0;
    int max_vertex_num = 0;
    int face_num = 0;
    int total_vertex_num = 0;
};