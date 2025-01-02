#include "texture.hpp"

Texture::Texture(){}

Texture::Texture(std::string path) {
    if(data != nullptr)free(data);
    LoadTexture(path);
}

Texture::~Texture() {
    if(data != nullptr)free(data);
}

void Texture::LoadTexture(std::string path) {
    data = stbi_load(path.c_str(), &width, &height, &n_channels, 0);
}

Vec3f Texture::Query(Vec2f uv) {
    // GL_REAPTE
    float u = uv(0) - int(uv(0));
    float v = uv(1) - int(uv(1));
    return GetColor(u * width, v * height);
}

Vec3f Texture::GetColor(int x, int y) {
    assert(data != nullptr);
    if(x < 0 || x >= width || y < 0 || y >= height)return Vec3f(0, 0, 0);
    int pos = n_channels * (y * width + x);
    Vec3f res;
    res(0) = data[pos], res(1) = data[pos + 1], res(2) = data[pos + 2];
    return res;
}