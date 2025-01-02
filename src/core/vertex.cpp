#include "vertex.hpp"

Vertex::Vertex(){
    position = {0, 0, 0};
    normal = {0, 0, 1};
    texture_coord = {0, 0};
    color = {0, 0, 0};
}

Vertex::Vertex(Vec3f &positon, Vec3f &normal, Vec2f &texture_coord, Vec3f &color):position(position), normal(normal), texture_coord(texture_coord), color(color){}