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
    zbuffer = new ZBuffer(width, height);
    hizbuffer = new HiZBuffer(width, height);

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

bool FrameBuffer::WriteZBuffer(int x, int y, float depth) {
    if(x < 0 || x >= width || y < 0 || y >= height)return false;
    if(hsrtype == ZBUFFER){
        assert(zbuffer != nullptr);
        return zbuffer -> WriteZBuffer(x, y, depth);
    }else if(hsrtype == HIZBUFFER || hsrtype == BVHHIZBUFFER){
        assert(hizbuffer != nullptr);
        return hizbuffer -> WirteZBuffer(x, y, depth);
    }
    return false;
}

bool FrameBuffer::Reject(Bounds2 triangle_bound) {
    assert(hizbuffer != nullptr);
    return hizbuffer -> Reject(triangle_bound);
}

void FrameBuffer::CleaZBuffer() {
    if(hsrtype == ZBUFFER){
        assert(zbuffer != nullptr);
        return zbuffer -> Clear();
    }else if(hsrtype == HIZBUFFER || hsrtype == BVHHIZBUFFER){
        assert(hizbuffer != nullptr);
        return hizbuffer -> Clear();
    }
}

void FrameBuffer::SetHSRType(HiddenSufaceRemovalType type) {
    hsrtype = type;
}
