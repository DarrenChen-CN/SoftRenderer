#include "glad/glad.h"
#include <iostream>
#include "GLFW/glfw3.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "window.hpp"
#include "renderer.hpp"
#include "model.hpp"
#include "framebuffer.hpp"
#include "shader.hpp"
#include "mathutil.hpp"
#include "scene.hpp"

int Scene::id = 0;


int main(void){
    // 初始化
    const int width = 800, height = 600;
    FrameBuffer *framebuffer = new FrameBuffer(width, height);
    Renderer *renderer = new Renderer(framebuffer);
    renderer->SetHSRType(ZBUFFER);
    // renderer->SetHSRType(HIZBUFFER);
    // renderer->SetHSRType(BVHHIZBUFFER);
    // renderer -> SetHSRType(SCANLINE);
    Window *window = new Window(width, height);
    window -> BindRenderer(renderer);

    // 配置五个场景
    Scene scene1, scene2, scene3, scene4, scene5;
    Model bunny("../../model/bunny144k.obj");
    Model head("../../model/african_head.obj");
    Model teapot("../../model/teapot15k.obj");
    Vec3f camera_pos = {0, 0, 3}, lookat = {0, 0, 0}, up = {0, 1, 0};
    float z_near = -0.1, z_far = -100, fovy = 45, aspect = 4.f/ 3;

    // 场景1 2k
    Shader scene1_head_shader;
    ShaderArgs scene1_head_args;
    scene1_head_args.viewport = GetViewPortMatrix(0, 0, width, height);
    scene1_head_args.view = GetViewMatrix(camera_pos, lookat, up);
    scene1_head_args.projection = GetPerspMatrix(fovy, aspect, z_near, z_far);
    scene1_head_shader.SetArgs(&scene1_head_args);
    head.SetShader(&scene1_head_shader);
    scene1.AddModel(head);

    // 场景2 15k
    Shader scene2_teapot_shader;
    ShaderArgs scene2_teapot_args;
    auto scale = Scale(Vec3f(0.6, 0.6, 0.6));
    auto translate = Translate(Vec3f(0.2, -0.4, 0));
    scene2_teapot_args.model = translate * scale;
    scene2_teapot_args.viewport = GetViewPortMatrix(0, 0, width, height);
    scene2_teapot_args.view = GetViewMatrix(camera_pos, lookat, up);
    scene2_teapot_args.projection = GetPerspMatrix(fovy, aspect, z_near, z_far);
    scene2_teapot_shader.SetArgs(&scene2_teapot_args);
    teapot.SetShader(&scene2_teapot_shader);
    scene2.AddModel(teapot);

    // 场景3 144k

    Shader scene3_bunny_shader;
    ShaderArgs scene3_bunny_args;
    scene3_bunny_args.viewport = GetViewPortMatrix(0, 0, width, height);
    scale = Scale(Vec3f(0.6, 0.6, 0.6));
    translate = Translate(Vec3f(0.2, -0.4, 0));
    scene3_bunny_args.model = translate * scale;
    scene3_bunny_args.view = GetViewMatrix(camera_pos, lookat, up);
    scene3_bunny_args.projection = GetPerspMatrix(fovy, aspect, z_near, z_far);
    scene3_bunny_shader.SetArgs(&scene3_bunny_args);
    bunny.SetShader(&scene3_bunny_shader);
    scene3.AddModel(bunny);

    // 场景4 遮挡场景 从后向前绘制
    Shader scene4_bunny_shander, scene4_head_shader;
    ShaderArgs scene4_bunny_args, scene4_head_args;
    
    scale = Scale(Vec3f(1.8, 1.8, 1.8));
    translate = Translate(Vec3f(0.2, -0.4, 0.3));
    scene4_head_args.viewport = GetViewPortMatrix(0, 0, width, height);
    scene4_head_args.model = translate * scale;
    scene4_head_args.view = GetViewMatrix(camera_pos, lookat, up);
    scene4_head_args.projection = GetPerspMatrix(fovy, aspect, z_near, z_far);
    scene4_head_shader.SetArgs(&scene4_head_args);
    head.SetShader(&scene4_head_shader);

    scene4.AddModel(bunny);
    scene4.AddModel(head);

    // 场景五 遮挡场景 从前向后绘制
    scene5.AddModel(head);
    scene5.AddModel(bunny);

    // 添加场景
    renderer -> AddScene(&scene1);
    renderer -> AddScene(&scene2);
    renderer -> AddScene(&scene3);
    renderer -> AddScene(&scene4);
    renderer -> AddScene(&scene5);
    

    // 修改场景
    // renderer -> SetSceneID(3);

    while(!window -> WindowShouldClose()){
        renderer -> ClearFrameBuffer();
        renderer -> ClearZBuffer();
        renderer -> Rendering();
        window -> Show();
    }

    window -> Quit();
    delete window;
    return 0;
}