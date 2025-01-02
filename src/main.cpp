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

int main(void){
    // 初始化
    const int width = 800, height = 600;
    FrameBuffer *framebuffer = new FrameBuffer(width, height);
    Renderer *render = new Renderer(framebuffer);
    Window *window = new Window(width, height);
    window -> BindRenderer(render);

    // 配置场景
    Model model("../../model/african_head/african_head.obj");
    Texture texture("../../model/african_head/african_head_diffuse.tga");
    Shader shader;
    ShaderArgs args;
    args.viewport = GetViewPortMatrix(0, 0, width, height);
    Vec3f camera_pos = {0, 0, 3}, lookat = {0, 0, 0}, up = {0, 1, 0};
    float z_near = -0.1, z_far = -100, fovy = 45, aspect = 4.f/ 3;
    args.view = GetViewMatrix(camera_pos, lookat, up);
    args.projection = GetPerspMatrix(fovy, aspect, z_near, z_far);
    args.texture = &texture;
    shader.SetArgs(&args);

    while(!window -> WindowShouldClose()){
        render -> Rendering(model, &shader);
        window -> Show();
    }

    window -> Quit();
    delete window;
    return 0;
}