#pragma once 
#include "define.hpp"
#include "vertex.hpp"
#include "framebuffer.hpp"
#include "model.hpp"
#include "triangle.hpp"
#include "shader.hpp"
#include "scene.hpp"
#include "bvh.hpp"

class Renderer{
public:
    Renderer();
    Renderer(FrameBuffer *framebuffer);
    ~Renderer();
    void BindFramebuffer(FrameBuffer *framebuffer);
    unsigned char *GetFrameBuffer();
    void SetHSRType(HiddenSufaceRemovalType type);
    int GetTotalRenderingFaces();
    int GetTotalSceneFaces();
    void ClearZBuffer();
    void Rendering();
    void AddScene(Scene *scene);
    void SetSceneID(int id);
    void ClearFrameBuffer();
    void SetBackFaceCulling(bool flag);

private:
    FrameBuffer *framebuffer = nullptr;
    VertexShaderOutput *vs_outputs = nullptr;
    int max_vertex_num = 0;
    int total_rendering_faces = 0;
    int total_scene_faces = 0;

    BVH *bvh = nullptr;

    HiddenSufaceRemovalType hsrtype = ZBUFFER;

    std::vector<Scene *> scene_list;
    int this_scene = 0; // 正在渲染的场景

    bool back_face_culling = false;

    void WriteBuffer(int x, int y, Vec3f color);
    bool WriteZBuffer(int x, int y, float depth);

    void RenderingScene(Scene *scene);
    int RenderingModel(Model &model);
    int RenderingTriangleMesh(TriangleMesh &mesh, Shader *shader); // 返回值为渲染的面片数
    int ScanLineRenderingScene(Scene *scene); // 使用scanline zbuffer渲染场景
    int BVHRenderingScene(Scene *scene); // 使用BVHHiZBuffer渲染场景
    void RenderingTriangle(Triangle &triangle, Shader *shader, VertexShaderOutput* vs_outputs); // 扫描线填充算法
    void RenderingTriangle(VertexShaderOutput v[3], Shader *shader);
    void UpTriangle(VertexShaderOutput &v1, VertexShaderOutput &v2, VertexShaderOutput &v3, Shader *shader); // 底边在下的平底三角形，其中v1要求为上顶点
    void DownTriangle(VertexShaderOutput &v1, VertexShaderOutput &v2, VertexShaderOutput &v3, Shader *shader); // 底边在上的平底三角形，其中v1要求为下顶点
    void ScanLine(VertexShaderOutput &v1, VertexShaderOutput &v2, Shader *shader);

    bool BackFaceCulling(VertexShaderOutput &v1, VertexShaderOutput &v2, VertexShaderOutput &v3); // 背面剔除

};