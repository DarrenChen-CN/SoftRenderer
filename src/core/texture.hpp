#pragma once 
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC 
#include "stb_image.h"
#include <string>
#include "define.hpp"

class Texture{
public:
    Texture();
    Texture(std::string path);
    ~Texture();
    Vec3f Query(Vec2f uv);
    void LoadTexture(std::string path);
    
private:
    int width, height, n_channels;
    unsigned char *data = nullptr;

    Vec3f GetColor(int x, int y);

};