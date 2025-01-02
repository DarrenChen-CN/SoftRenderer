#pragma once 
#include "define.hpp"
#include "vertex.hpp"
#include "framebuffer.hpp"
#include "model.hpp"
#include "triangle.hpp"
#include "shader.hpp"

class Renderer{
public:
    Renderer();
    Renderer(FrameBuffer *framebuffer);
    ~Renderer();
    void BindFramebuffer(FrameBuffer *framebuffer);
    unsigned char *GetFrameBuffer();

    void Rendering(Model &model, Shader *shader);
private:
    FrameBuffer *framebuffer = nullptr;

    void WriteBuffer(int x, int y, Vec3f color);
    bool WriteZBuffer(int x, int y, float depth);
    void ClearZBuffer();

    void RenderingTriangleMesh(TriangleMesh &mesh, Shader *shader);

    void RenderingLine(); // todo Bresenham算法
    void RenderingTriangle(Triangle &triangle, Shader *shader); // 扫描线填充算法
    void UpTriangle(VertexShaderOutput &v1, VertexShaderOutput &v2, VertexShaderOutput &v3, Shader *shader); // 底边在下的平底三角形，其中v1要求为上顶点
    void DownTriangle(VertexShaderOutput &v1, VertexShaderOutput &v2, VertexShaderOutput &v3, Shader *shader); // 底边在上的平底三角形，其中v1要求为下顶点
    void ScanLine(VertexShaderOutput &v1, VertexShaderOutput &v2, Shader *shader);

};