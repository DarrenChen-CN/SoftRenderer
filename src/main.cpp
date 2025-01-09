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
    renderer->SetHSRType(HIZBUFFER);
    renderer->SetHSRType(BVHHIZBUFFER);
    renderer -> SetHSRType(SCANLINE);
    Window *window = new Window(width, height);
    window -> BindRenderer(renderer);

    // 配置场景
    Scene scene;
    Model bunny("../../model/bunny/bunny144k.obj");
    Model head("../../model/african_head/african_head.obj");
    Model head2("../../model/african_head/african_head.obj");

    Shader bunny_shader, head_shader, head2_shader;
    ShaderArgs bunny_args, head_args, head2_args;
    bunny_args.viewport = GetViewPortMatrix(0, 0, width, height);
    auto scale = Scale(Vec3f(0.6, 0.6, 0.6));
    auto translate = Translate(Vec3f(0.2, -0.4, 0));
    bunny_args.model = translate * scale;
    Vec3f camera_pos = {0, 0, 3}, lookat = {0, 0, 0}, up = {0, 1, 0};
    float z_near = -0.1, z_far = -100, fovy = 45, aspect = 4.f/ 3;
    bunny_args.view = GetViewMatrix(camera_pos, lookat, up);
    bunny_args.projection = GetPerspMatrix(fovy, aspect, z_near, z_far);
    bunny_shader.SetArgs(&bunny_args);
    bunny.SetShader(&bunny_shader);
    
    scale = Scale(Vec3f(1.5, 1.5, 1.5));
    // translate = Translate(Vec3f(0.4, -0.0, -0.0));
    translate = Translate(Vec3f(0.4, -0.0, -0.3));
    head_args.viewport = GetViewPortMatrix(0, 0, width, height);
    head_args.model = translate * scale;
    head_args.view = GetViewMatrix(camera_pos, lookat, up);
    head_args.projection = GetPerspMatrix(fovy, aspect, z_near, z_far);
    head_shader.SetArgs(&head_args);
    head.SetShader(&head_shader);

    scale = Scale(Vec3f(1.8, 1.8, 1.8));
    translate = Translate(Vec3f(0.2, -0.4, 0.3));
    head2_args.viewport = GetViewPortMatrix(0, 0, width, height);
    head2_args.model = translate * scale;
    head2_args.view = GetViewMatrix(camera_pos, lookat, up);
    head2_args.projection = GetPerspMatrix(fovy, aspect, z_near, z_far);
    head2_shader.SetArgs(&head2_args);
    head2.SetShader(&head2_shader);
    // scene.AddModel(bunny);
    // scene.AddModel(head2);
    scene.AddModel(head);
    
    
    // scene.AddModel(head);

    while(!window -> WindowShouldClose()){
        renderer -> ClearZBuffer();
        renderer -> RenderingScene(&scene);
        window -> Show();
    }

    window -> Quit();
    delete window;
    return 0;
}