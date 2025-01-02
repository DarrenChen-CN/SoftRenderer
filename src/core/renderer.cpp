#include "renderer.hpp"
#include "shader.hpp"
#include <assert.h>
#include <algorithm>
#include "mathutil.hpp"
#include <iostream>

Renderer::Renderer(){}

Renderer::Renderer(FrameBuffer *framebuffer){
    this -> framebuffer = framebuffer;
}

Renderer::~Renderer(){
    if(framebuffer != nullptr)delete framebuffer;
}

void Renderer::BindFramebuffer(FrameBuffer *bramebuffer){
    this -> framebuffer = framebuffer;
}

unsigned char *Renderer::GetFrameBuffer(){
    assert(framebuffer != nullptr);
    return framebuffer -> GetFrameBuffer();
}

bool mycmp(VertexShaderOutput &v1, VertexShaderOutput &v2){
    return v1.screen_position(1) < v2.screen_position(1);
}

void Renderer::RenderingTriangle(Triangle &triangle, Shader* shader){
    assert(shader != nullptr);
    // 顶点输入顶点着色器
    VertexShaderOutput v[3];
    v[0] = shader -> VertexShader(triangle.GetVertex(0));
    v[1] = shader -> VertexShader(triangle.GetVertex(1));
    v[2] = shader -> VertexShader(triangle.GetVertex(2));

    // 按高度进行排序
    std::sort(v, v + 3, mycmp);

    // 底边在下的平边三角形
    if(v[1].screen_position(1) == v[0].screen_position(1)){
        UpTriangle(v[2], v[0], v[1], shader);
        return;
    }
    // 底边在上的平边三角形
    if(v[1].screen_position(1) == v[2].screen_position(1)){
        DownTriangle(v[1], v[0], v[2], shader);
        return;
    }


    // 根据第二高的顶点将三角形分成两部分
    float factor = (v[2].screen_position(1) - v[1].screen_position(1)) / (v[2].screen_position(1) - v[0].screen_position(1));
    VertexShaderOutput middle = VertexShaderOutput::Lerp(v[2], v[0], factor);
    UpTriangle(v[2], v[1], middle, shader);
    DownTriangle(v[0], v[1], middle, shader);
}

void Renderer::UpTriangle(VertexShaderOutput &v1, VertexShaderOutput &v2, VertexShaderOutput &v3, Shader *shader){
    assert(shader != nullptr);
    VertexShaderOutput top = v1;
    VertexShaderOutput left = v2.screen_position(0) > v3.screen_position(0) ? v3 : v2;
    VertexShaderOutput right = v2.screen_position(0) > v3.screen_position(0) ? v2 : v3;
    left.screen_position(0) = int(left.screen_position(0));
    int dy = top.screen_position(1) - left.screen_position(1);
    int now_y = top.screen_position(1);
    // 从上往下
    for(int i = dy; i >= 0; i --){
        float factor;
        if(dy != 0){
            factor = i * 1.f / dy;
        }
        VertexShaderOutput new_left = VertexShaderOutput::Lerp(left, top, factor);
        VertexShaderOutput new_right = VertexShaderOutput::Lerp(right, top, factor);
        new_left.screen_position(0) = int(new_left.screen_position(0));
        new_right.screen_position(0) = int(new_right.screen_position(0) + 0.5);
        new_left.screen_position(1) = new_right.screen_position(1) = now_y;
        ScanLine(new_left, new_right, shader);
        now_y --;
    }

}

void Renderer::DownTriangle(VertexShaderOutput &v1, VertexShaderOutput &v2, VertexShaderOutput &v3, Shader *shader){
    assert(shader != nullptr);
    VertexShaderOutput bottom = v1;
    VertexShaderOutput left = v2.screen_position(0) > v3.screen_position(0) ? v3 : v2;
    VertexShaderOutput right = v2.screen_position(0) > v3.screen_position(0) ? v2 : v3;
    left.screen_position(0) = int(left.screen_position(0));
    int dy = left.screen_position(1) - bottom.screen_position(1);
    int now_y = left.screen_position(1);
    // 从上往下
    for(int i = 0; i < dy; i ++){
        float factor;
        if(dy != 0){
            factor = i * 1.f / dy;
        }
        VertexShaderOutput new_left = VertexShaderOutput::Lerp(left, bottom, factor);
        VertexShaderOutput new_right = VertexShaderOutput::Lerp(right, bottom, factor);
        new_left.screen_position(0) = int(new_left.screen_position(0));
        new_right.screen_position(0) = int(new_right.screen_position(0) + 0.5);
        new_left.screen_position(1) = new_right.screen_position(1) = now_y;
        ScanLine(new_left, new_right, shader);
        now_y --;
    }
}

void Renderer::ScanLine(VertexShaderOutput &left, VertexShaderOutput &right, Shader *shader){
    assert(shader != nullptr);
    int len = right.screen_position(0) - left.screen_position(0);
    // std::cout << left.screen_position << std::endl;
    for(int i = 0; i < len; i ++){
        VertexShaderOutput v = VertexShaderOutput::Lerp(left, right, i * 1.f / len);
        int x = left.screen_position(0) + i;
        int y = left.screen_position(1);
        // std::cout << v.ndc_position(2) << std::endl;
        bool flag = WriteZBuffer(x, y, v.ndc_position(2));
        if(!flag)continue;
        Vec3f color = shader -> FragmentShader(v);
        // std::cout << x << " " << y << " " << color(0) << ", " << color(1) << ", " << color(2) << std::endl;
        WriteBuffer(x, y, color);
    }
}

void Renderer::WriteBuffer(int x, int y, Vec3f color){
    framebuffer -> WriteBuffer(x, y, color);
}

bool Renderer::WriteZBuffer(int x, int y, float depth) {
    return framebuffer -> WriteZBuffer(x, y, depth);
}

void Renderer::ClearZBuffer() {
    framebuffer -> ClearZBuffer();
}

void Renderer::RenderingTriangleMesh(TriangleMesh &mesh, Shader *shader) {
    assert(shader != nullptr);
    auto indices = mesh.GetIndices();
    auto face_num = mesh.GetFaceNum();
    for(int i = 0; i < face_num; i ++){
        Triangle tri(indices[3 * i], indices[3 * i + 1], indices[3 * i + 2], &mesh);
        RenderingTriangle(tri, shader);
    }
}

void Renderer::Rendering(Model &model, Shader *shader){
    ClearZBuffer();
    auto meshes = model.GetMeshes();
    for(auto& mesh: meshes){
        RenderingTriangleMesh(mesh, shader);
    }
}

