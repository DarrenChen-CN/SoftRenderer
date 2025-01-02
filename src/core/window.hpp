#pragma once 
#include "glad/glad.h"
#include <iostream>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "renderer.hpp"

class Window{

public: 
    Window();
    Window(int width, int height);
    ~Window();
    void BindRenderer(Renderer *renderer);
    void Init();
    void Show();
    void Quit();
    int Width();
    int Height();
    void SetWidth(int width);
    void SetHeight(int height);
    bool WindowShouldClose();

private:
    int width = 800, height = 600; 
    Renderer *renderer = nullptr;
    GLFWwindow *window = nullptr;
    GLuint vao, vbo, pbo, screen_texture, screen_prog_id;
};

