#include "shader.hpp"
#include "mathutil.hpp"
#include <iostream>

VertexShaderOutput Shader::VertexShader(Vertex v){
    VertexShaderOutput vs_output;
    Vec4f pos = Vec3ToVec4(v.position);
    vs_output.world_position = args -> model * pos;
    vs_output.ndc_position =  args -> projection * args -> view * args -> model * pos;
    vs_output.ndc_position = PerspectiveDivision(vs_output.ndc_position);
    vs_output.screen_position = args -> viewport * vs_output.ndc_position;
    vs_output.normal = v.normal;
    vs_output.texture_coord = v.texture_coord;
    vs_output.vertex_color = v.color;
    return vs_output;
}

Vec3f Shader::FragmentShader(VertexShaderOutput vs_output){
    if(args -> texture != nullptr)return args->texture->Query(vs_output.texture_coord);
    Vec3f light_dir = (args->light_pos - Vec3f(vs_output.world_position(0), vs_output.world_position(1), vs_output.world_position(2))).normalized();
    float intensity = std::max(0.f, light_dir.dot(vs_output.normal.normalized()));
    return intensity * args->light_color;
}