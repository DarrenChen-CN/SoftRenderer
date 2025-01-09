#pragma once
#include "define.hpp"

class ZBuffer{
public:
    ZBuffer();
    ZBuffer(int width, int height);
    ~ZBuffer();
    void Clear();
    bool WriteZBuffer(int x, int y, float depth);

private:
    int width, height;
    float *z_buffer = nullptr;

};