#include "renderer.hpp"
#include "shader.hpp"
#include <assert.h>
#include <algorithm>
#include "mathutil.hpp"
#include <iostream>
#include "GLFW/glfw3.h"
#include <omp.h>

Renderer::Renderer(){}

Renderer::Renderer(FrameBuffer *framebuffer){
    this -> framebuffer = framebuffer;
}

Renderer::~Renderer(){
    if(framebuffer != nullptr)delete framebuffer;
    if(vs_outputs != nullptr)delete[] vs_outputs;
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

void Renderer::RenderingTriangle(Triangle &triangle, Shader* shader, VertexShaderOutput *vs_outputs){
    assert(shader != nullptr);

    // 顶点输入顶点着色器
    VertexShaderOutput v[3];
    v[0] = vs_outputs[triangle.GetIndex(0)];
    v[1] = vs_outputs[triangle.GetIndex(1)];
    v[2] = vs_outputs[triangle.GetIndex(2)];

    RenderingTriangle(v, shader);
}

void Renderer::RenderingTriangle(VertexShaderOutput v[3], Shader *shader) {
    // 按高度进行排序
    std::sort(v, v + 3, mycmp);

    // 底边在下的平边三角形
    if(fabs(v[1].screen_position(1) - v[0].screen_position(1)) < eps){
        // std::cout << 111 << std::endl;
        UpTriangle(v[2], v[0], v[1], shader);
    }
    // 底边在上的平边三角形
    if(fabs(v[1].screen_position(1) - v[2].screen_position(1)) < eps){
        // std::cout << 111 << std::endl;
        DownTriangle(v[1], v[0], v[2], shader);
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
    int dy = top.screen_position(1) - left.screen_position(1) + 0.5;
    int now_y = top.screen_position(1) + 0.5;
    // 从上往下
    for(int i = dy; i >= 0; i --){
        float factor;
        if(dy != 0){
            factor = i * 1.f / dy;
        }
        VertexShaderOutput new_left = VertexShaderOutput::Lerp(left, top, factor);
        VertexShaderOutput new_right = VertexShaderOutput::Lerp(right, top, factor);
        new_left.screen_position(0) = int(new_left.screen_position(0) - 0.5);
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
    int dy = left.screen_position(1) - bottom.screen_position(1) + 0.5;
    int now_y = left.screen_position(1) + 0.5;
    // 从上往下
    for(int i = 0; i < dy; i ++){
        float factor;
        if(dy != 0){
            factor = i * 1.f / dy;
        }
        VertexShaderOutput new_left = VertexShaderOutput::Lerp(left, bottom, factor);
        VertexShaderOutput new_right = VertexShaderOutput::Lerp(right, bottom, factor);
        new_left.screen_position(0) = int(new_left.screen_position(0) - 0.5);
        new_right.screen_position(0) = int(new_right.screen_position(0) + 0.5);
        new_left.screen_position(1) = new_right.screen_position(1) = now_y;
        ScanLine(new_left, new_right, shader);
        now_y --;
    }
}

void Renderer::ScanLine(VertexShaderOutput &left, VertexShaderOutput &right, Shader *shader){
    assert(shader != nullptr);
    int len = right.screen_position(0) - left.screen_position(0) + 0.5;
    // std::cout << left.screen_position << std::endl;
    for(int i = 0; i < len; i ++){
        VertexShaderOutput v = VertexShaderOutput::Lerp(left, right, i * 1.f / len);
        int x = v.screen_position(0) - 0.5;
        int y = v.screen_position(1) - 0.5;
        if(!framebuffer -> WriteZBuffer(x, y, v.screen_position(2)))continue;
        Vec3f color = shader -> FragmentShader(v);
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
    framebuffer -> CleaZBuffer();
}

int Renderer::RenderingTriangleMesh(TriangleMesh &mesh, Shader *shader) {
    float start, end;
    // auto shader = mesh.GetShader();
    assert(shader != nullptr);
    auto indices = mesh.GetIndices();
    auto face_num = mesh.GetFaceNum();
    auto vertices = mesh.GetVertices();
    // 顶点着色器
    
    for(int i = 0; i < vertices.size(); i ++){
        auto v = vertices[i];
        vs_outputs[i] = shader -> VertexShader(v);
    }
    // 光栅化
    int faces = 0;
    for(int i = 0; i < face_num; i ++){
        Triangle tri(i, indices[3 * i], indices[3 * i + 1], indices[3 * i + 2], &mesh);
        // 背面剔除
        // int idx0 = tri.GetIndex(0), idx1 = tri.GetIndex(1), idx2 = tri.GetIndex(2);
        // bool flag = BackFaceCulling(vs_outputs[idx0], vs_outputs[idx1], vs_outputs[idx2]);
        // if(flag)continue;
        // HiZ
        if(hsrtype == HIZBUFFER){
            // Bounds2d triangle_bound(vs_outputs[tri.GetIndex(0)].screen_position, vs_outputs[tri.GetIndex(1)].screen_position, vs_outputs[tri.GetIndex(2)].screen_position);
            Bounds2 triangle_bound(vs_outputs[indices[3 * i]].screen_position, vs_outputs[indices[3 * i + 1]].screen_position, vs_outputs[indices[3 * i + 2]].screen_position);
            if(framebuffer -> Reject(triangle_bound))continue;
        }
        faces ++;
        RenderingTriangle(tri, shader, vs_outputs);
    } 
    return faces;
}

void Renderer::RenderingScene(Scene *scene) {
    assert(scene != nullptr);
    total_scene_faces = scene->GetFaceNum();
    int scene_total_vertex_num = scene -> GetTotalVertexNum();
    if(scene_total_vertex_num > max_vertex_num){
        if(vs_outputs != nullptr)delete[] vs_outputs;
        vs_outputs = new VertexShaderOutput[scene_total_vertex_num];
    }
    if(hsrtype == BVHHIZBUFFER){
        this -> total_rendering_faces = BVHRenderingScene(scene);
    }else if(hsrtype == ZBUFFER || hsrtype == HIZBUFFER){
        auto models = scene -> GetModels();
        int total_rendering_faces = 0;
        for(Model &model: models){
            total_rendering_faces += RenderingModel(model);
        }
        this -> total_rendering_faces = total_rendering_faces;
    }else{
        this -> total_rendering_faces = ScanLineRenderingScene(scene);
    }
    
}

int Renderer::RenderingModel(Model &model){
    auto meshes = model.GetMeshes();
    int total_rendering_faces = 0;
    for(auto& mesh: meshes){
        total_rendering_faces += RenderingTriangleMesh(mesh, model.GetShader());
    }
    return total_rendering_faces;
}

int Renderer::BVHRenderingScene(Scene *scene) {
    // 判断是否需要bvh初始化 此处只渲染静态场景 故只需要构建一次 
    if(bvh == nullptr || bvh -> GetID() != scene->GetID()){
        if(bvh)delete bvh;
        // 初始化
        std::vector<BVHTriangleInfo> triangle_info(scene->GetFaceNum());
        // 先对所有顶点进行坐标变换 并组装三角形
        int vertex_offset = 0, face_offset = 0;
        auto models = scene->GetModels();
        for(auto &model: models){
            auto meshes = model.GetMeshes();
            auto shader = model.GetShader();
            assert(shader != nullptr);
            for(auto &mesh: meshes){
                auto indices = mesh.GetIndices();
                auto face_num = mesh.GetFaceNum();
                auto vertices = mesh.GetVertices();
                // 顶点着色器
                for(int i = 0; i < vertices.size(); i ++){
                    auto v = vertices[i];
                    vs_outputs[i + vertex_offset] = shader -> VertexShader(v);
                }
                // 组装三角形
                for(int i = 0; i < face_num; i ++){
                    triangle_info[i + face_offset] = {vs_outputs[indices[3 * i] + vertex_offset], vs_outputs[indices[3 * i + 1] + vertex_offset], vs_outputs[indices[3 * i + 2] + vertex_offset], shader};
                    // std::cout << triangle_info[i + face_offset].bounds.p_min << std::endl;
                }
                vertex_offset += vertices.size();
                face_offset += face_num;
            }
        }
        // 构建bvh
        // std::cout << triangle_info.size() << std::endl;
        bvh = new BVH(scene->GetID(), triangle_info);
    }
    // 已拥有bvh 根据bvh的叶节点进行光栅化
    auto leaf_nodes = bvh -> GetLeafNode();
    auto triangles_info = bvh -> GetTriangleInfo();
    // 从近到远光栅化
    int num_face = leaf_nodes.size();
    int faces = 0;
    // std::cout << num_face << std::endl;
    
    for(int i = 0; i < num_face; i ++){
        
        auto triangle_info = triangles_info[leaf_nodes[i] -> triangle_offset];
        // 背面剔除
        // bool flag = BackFaceCulling(triangle_info.vs_output[0], triangle_info.vs_output[1], triangle_info.vs_output[2]);
        // if(flag)continue;
        // HiZ
        Bounds2 triangle_bound(triangle_info.vs_output[0].screen_position, triangle_info.vs_output[1].screen_position, triangle_info.vs_output[2].screen_position);
        if(framebuffer -> Reject(triangle_bound))continue;
        faces ++;
        
        RenderingTriangle(triangle_info.vs_output, triangle_info.shader);
        
    }
    return faces;
}

int Renderer::ScanLineRenderingScene(Scene *scene){
    // 判断是否需要初始化
    int scene_id = framebuffer -> ScanLineSceneId();
    if(scene -> id != scene_id){
        std::vector<ScanLineTriangleInfo> triangle_info(scene->GetFaceNum()); 
        // 先对所有顶点进行坐标变换 并组装三角形
        int vertex_offset = 0, face_offset = 0;
        auto models = scene->GetModels();
        for(auto &model: models){
            auto meshes = model.GetMeshes();
            auto shader = model.GetShader();
            assert(shader != nullptr);
            for(auto &mesh: meshes){
                auto indices = mesh.GetIndices();
                auto face_num = mesh.GetFaceNum();
                auto vertices = mesh.GetVertices();
                // 顶点着色器
                for(int i = 0; i < vertices.size(); i ++){
                    auto v = vertices[i];
                    vs_outputs[i + vertex_offset] = shader -> VertexShader(v);
                }
                // 组装三角形
                for(int i = 0; i < face_num; i ++){
                    triangle_info[i + face_offset] = {i + face_offset, vs_outputs[indices[3 * i] + vertex_offset], vs_outputs[indices[3 * i + 1] + vertex_offset], vs_outputs[indices[3 * i + 2] + vertex_offset], shader};
                    // std::cout << triangle_info[i + face_offset].bounds.p_min << std::endl;
                }
                vertex_offset += vertices.size();
                face_offset += face_num;
            }
        }
        framebuffer -> ScanLineInit(scene -> id, triangle_info);
    }
    // std::cout << 111 << std::endl;
    framebuffer -> Scan();
    return scene->GetFaceNum();
}

bool Renderer::BackFaceCulling(VertexShaderOutput &v1, VertexShaderOutput &v2, VertexShaderOutput &v3) {
    // 使用NDC坐标
    auto ndc_v1 = v1.ndc_position, ndc_v2 = v2.ndc_position, ndc_v3 = v3.ndc_position;
    Vec3f t1 = {ndc_v2(0) - ndc_v1(0), ndc_v2(1) - ndc_v1(1), ndc_v2(2) - ndc_v1(2)};
    Vec3f t2 = {ndc_v3(0) - ndc_v1(0), ndc_v3(1) - ndc_v1(1), ndc_v3(2) - ndc_v1(2)};
    Vec3f normal = t1.cross(t2).normalized();
    Vec3f view = {0, 0, 1}; // ndc空间观察方向
    return view.dot(normal) < 0;
}

void Renderer::SetHSRType(HiddenSufaceRemovalType type) {
    hsrtype = type;
    framebuffer -> SetHSRType(type);
}

int Renderer::GetTotalRenderingFaces() {
    return total_rendering_faces;
}

int Renderer::GetTotalSceneFaces() {
    return total_scene_faces;
}
