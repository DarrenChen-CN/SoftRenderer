#pragma once
#include "define.hpp"

class FrameBuffer{
public:
    FrameBuffer();
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    int Width();
    int Height();
    void SetWidth(int width);
    void SetHeight(int height);
    void Init();
    unsigned char *GetFrameBuffer();

    void WriteBuffer(int x, int y, Vec3f color);
    void ClearZBuffer();
    bool WriteZBuffer(int x, int y, float depth); // 返回值表示是否修改zbuffer true为修改

private:
    int width = 800, height = 600;
    unsigned char *framebuffer = nullptr;
    float *zbuffer = nullptr;
};