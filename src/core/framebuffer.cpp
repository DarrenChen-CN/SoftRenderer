#include "framebuffer.hpp"
#include "assert.h"
#include <iostream>
#include "define.hpp"

FrameBuffer::FrameBuffer(){

}

FrameBuffer::FrameBuffer(int width, int height):width(width), height(height){
    Init();
} 

FrameBuffer::~FrameBuffer(){
    if(framebuffer != nullptr)delete[] framebuffer;
    if(zbuffer != nullptr)delete[] zbuffer;
}

void FrameBuffer::Init(){
    assert(width > 0 && height > 0);
    framebuffer = new unsigned char[width * height * 3];
    zbuffer = new float[width * height];
    ClearZBuffer();

}

unsigned char * FrameBuffer::GetFrameBuffer(){
    assert(framebuffer != nullptr);
    return framebuffer;
}

int FrameBuffer::Width(){
    return width;
}

int FrameBuffer::Height(){
    return height;
}

void FrameBuffer::SetWidth(int width){
    assert(width > 0);
    this -> width = width;
}

void FrameBuffer::SetHeight(int height){
    assert(height > 0);
    this -> height = height;
}

void FrameBuffer::WriteBuffer(int x, int y, Vec3f color) {
    if(x < 0 || x >= width || y < 0 || y >= height)return ;
    y = height - y - 1;
    // std::cout << x << " " << y << std::endl;
    // std::cout << x << " " << y << std: endl;
    framebuffer[3 * (y * width + x)] = color(0);
    framebuffer[3 * (y * width + x) + 1] = color(1);
    framebuffer[3 * (y * width + x) + 2] = color(2);
}

void FrameBuffer::ClearZBuffer() {
    assert(zbuffer != nullptr);
    for(int i = 0; i < width * height; i ++)zbuffer[i] = -1; // 在ndc空间使用zbuffer 最远为-1
}

bool FrameBuffer::WriteZBuffer(int x, int y, float depth) {
    assert(zbuffer != nullptr);
    if(x < 0 || x >= width || y < 0 || y >= height)return false;
    y = height - y - 1;
    int pos = y * width + x;
    if(zbuffer[pos] < depth){
        zbuffer[pos] = depth;
        return true;
    }
    return false;
}