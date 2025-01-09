#include "scene.hpp"
#include <assert.h>
#include <iostream>

Scene::Scene(){
    scene_id = id++;
}

int Scene::AddModel(Model &model) {
    int res = model_num;
    models.push_back(model);
    model_num++;
    face_num += model.GetFaceNum();
    max_vertex_num = std::max(max_vertex_num, model.GetMaxVertexNum());
    total_vertex_num += model.GetTotalVertexNum();
    // std::cout << max_vertex_num << " " << total_vertex_num << std::endl;
    return res;
}

Model &Scene::GetModel(int id) {
    assert(id < model_num && id >= 0);
    return models[id];
}

int Scene::GetModelNum() {
    return model_num;
}

std::vector<Model> Scene::GetModels() {
    return models;
}

int Scene::GetMaxVertexNum() {
    return max_vertex_num;
}

int Scene::GetFaceNum() {
    return face_num;
}

int Scene::GetTotalVertexNum() {
    return total_vertex_num;
}

int Scene::GetID() {
    return scene_id;
}