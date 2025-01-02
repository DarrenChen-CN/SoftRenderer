#pragma once
#include "define.hpp"
#include "vertex.hpp"
#include "texture.hpp"


class VertexShaderOutput{
public:
    Vec4f world_position;
    Vec4f ndc_position;
    Vec4f screen_position;
    Vec2f texture_coord;
    Vec3f normal;
    Vec3f vertex_color;

    VertexShaderOutput(){};
    ~VertexShaderOutput(){};
    static VertexShaderOutput Lerp(VertexShaderOutput &a, VertexShaderOutput &b, float factor){
        VertexShaderOutput res;

        res.world_position = (1 - factor) * a.world_position + factor * b.world_position;
        res.ndc_position = (1 - factor) * a.ndc_position + factor * b.ndc_position;
        res.screen_position = (1 - factor) * a.screen_position + factor * b.screen_position;
        res.normal = (1 - factor) * a.normal + factor * b.normal;
        res.vertex_color = (1 - factor) * a.vertex_color + factor * b.vertex_color;
        res.texture_coord = (1 - factor) * a.texture_coord + factor * b.texture_coord;

        return res;
    }
};

class ShaderArgs{
public:
    ShaderArgs(){};
    ~ShaderArgs(){};

    Mat4f model = Mat4f::Identity();
    Mat4f view = Mat4f::Identity();
    Mat4f projection = Mat4f::Identity();
    Mat4f viewport = Mat4f::Identity();

    Texture *texture = nullptr;
};



class Shader{
public:
    Shader(){}
    ~Shader(){}
    VertexShaderOutput VertexShader(Vertex v);

    Vec3f FragmentShader(VertexShaderOutput vs_output);
    void SetArgs(ShaderArgs *args_){
        args = args_;
    }

    ShaderArgs *GetArgsPtr(){
        assert(args != nullptr);
        return args;
    }

protected:
    ShaderArgs *args = nullptr;
};