#include "zbuffer.hpp"

ZBuffer::ZBuffer(){}

ZBuffer::ZBuffer(int width, int height): width(width), height(height){
    assert(width > 0 && height > 0);
    z_buffer = new float[width * height];
    Clear();
}

ZBuffer::~ZBuffer() {
    if(z_buffer != nullptr)delete[] z_buffer;
}

void ZBuffer::Clear() {
    assert(z_buffer != nullptr);
    for(int i = 0; i < width * height; i ++){
        z_buffer[i] = -1;
    }
}

bool ZBuffer::WriteZBuffer(int x, int y, float depth) {
    if(x < 0 || y < 0 || x >= width || y >= height)return false;
    y = height - y - 1;
    int pos = y * width + x;
    if(z_buffer[pos]> depth)return false; // 不需要修改
    z_buffer[pos] = depth;
    return true;
}