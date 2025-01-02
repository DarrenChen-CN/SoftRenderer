#pragma once 
#include "define.hpp"

struct Vertex{
    Vec3f position;
    Vec3f normal;
    Vec2f texture_coord;
    Vec3f color;

    Vertex();
    Vertex(Vec3f &positon, Vec3f &normal, Vec2f &texture_coord, Vec3f &color);
};