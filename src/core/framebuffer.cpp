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
    if(hi_zbuffer != nullptr)delete hi_zbuffer;
    if(scanline_zbuffer != nullptr)delete scanline_zbuffer;
}

void FrameBuffer::Init(){
    assert(width > 0 && height > 0);
    framebuffer = new unsigned char[width * height * 3];
    zbuffer = new ZBuffer(width, height);
    hi_zbuffer = new HiZBuffer(width, height);
    scanline_zbuffer = new ScanLineZBuffer(width, height);
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
        assert(hi_zbuffer != nullptr);
        return hi_zbuffer -> WirteZBuffer(x, y, depth);
    }
    return false;
}

bool FrameBuffer::Reject(Bounds2 triangle_bound) {
    assert(hi_zbuffer != nullptr);
    return hi_zbuffer -> Reject(triangle_bound);
}

void FrameBuffer::CleaZBuffer() {
    if(hsrtype == ZBUFFER){
        assert(zbuffer != nullptr);
        return zbuffer -> Clear();
    }else if(hsrtype == HIZBUFFER || hsrtype == BVHHIZBUFFER){
        assert(hi_zbuffer != nullptr);
        return hi_zbuffer -> Clear();
    }else {
        // scanline 扫描前会clear
        return;
    }
}

void FrameBuffer::ClearBuffer() {
    memset(framebuffer, 0, sizeof(unsigned char) * width * height * 3);
    // std::cout << 111 << std::endl;
}

void FrameBuffer::SetHSRType(HiddenSufaceRemovalType type) {
    hsrtype = type;
}

void FrameBuffer::ScanLineInit(int id, std::vector<ScanLineTriangleInfo> &tris){
    assert(scanline_zbuffer != nullptr);
    scanline_zbuffer -> Init(id, tris);
}

int FrameBuffer::ScanLineSceneId(){
    assert(scanline_zbuffer != nullptr);
    return scanline_zbuffer -> GetSceneID();
}

void FrameBuffer::Scan(){
    assert(scanline_zbuffer != nullptr);
    scanline_zbuffer -> Scan(framebuffer);
}
