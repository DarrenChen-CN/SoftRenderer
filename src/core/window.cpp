#include "window.hpp"

Window::Window(){
    Init();
}

Window::Window(int width, int height):width(width), height(height){
    Init();
}

Window::~Window(){
    if(renderer != nullptr)delete renderer;
}

void Window::BindRenderer(Renderer *renderer){
    assert(renderer != nullptr);
    this -> renderer = renderer;
}

int Window::Width(){
    return width;
}

int Window::Height(){
    return height;
}

void Window::SetWidth(int width){
    assert(width > 0);
    this -> width = width;
}

void Window::SetHeight(int height){
    assert(height > 0);
    this -> height = height;
}

void Window::Init(){
    assert(width > 0 && height > 0);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "SoftRenderer", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return ;
    }
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return ;
    }
    glViewport(0, 0, width, height);

    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 3, 0, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    float vertices[] = {
        // positions        // texture coords
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right

        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // top left
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const char *vertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        void main()
        {
            gl_Position = vec4(aPos, 1.0);
            TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
        })glsl";

    const char *fragmentShaderSource = R"glsl(
        #version 330 core
        out vec4 FragColor;

        in vec2 TexCoord;

        uniform sampler2D texture1;

        void main()
        {
            FragColor = texture(texture1, TexCoord);
        })glsl";

    GLint vs_id = glCreateShader(GL_VERTEX_SHADER);
    GLint fs_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vs_id, 1, &vertexShaderSource, nullptr);
    glShaderSource(fs_id, 1, &fragmentShaderSource, nullptr);
    
    screen_prog_id = glCreateProgram();
    
    glCompileShader(vs_id);
    glCompileShader(fs_id);
    
    glAttachShader(screen_prog_id, vs_id);
    glAttachShader(screen_prog_id, fs_id);
    
    glLinkProgram(screen_prog_id);
    glValidateProgram(screen_prog_id);

    glDeleteShader(vs_id);
    glDeleteShader(fs_id);

    glUseProgram(screen_prog_id);
}


void Window::Show(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("SoftRenderer");
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(400, 70));
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("total scene faces: %d, total rendering faces: %d", renderer -> GetTotalSceneFaces(), renderer -> GetTotalRenderingFaces());
    ImGui::End();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 复制framebuffer到pbo
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 3, 0, GL_STREAM_DRAW);
    GLubyte *bufPtr = (GLubyte *) glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0,
                                        width * height * 3,
                                        GL_MAP_WRITE_BIT |
                                        GL_MAP_INVALIDATE_BUFFER_BIT);

    if(bufPtr){
        memcpy(bufPtr, renderer -> GetFrameBuffer(), static_cast<size_t>(width * height * 3));
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }else {
        std::cout << "error!!!\n" << std::endl;
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window); 
    glfwSwapInterval(0); // 关闭垂直同步
    glfwPollEvents();
}

void Window::Quit() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &pbo);
    glDeleteTextures(1, &screen_texture);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::WindowShouldClose() {
    assert(window != nullptr);
    return glfwWindowShouldClose(window);
}

