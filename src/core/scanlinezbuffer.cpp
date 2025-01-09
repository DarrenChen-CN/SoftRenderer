#include "scanlinezbuffer.hpp"
#include <iostream>
#include <cstring>

ScanLineZBuffer::ScanLineZBuffer(){

}

ScanLineZBuffer::ScanLineZBuffer(int width, int height): width(width), height(height){
    z_buffer = new float[width];
}

ScanLineZBuffer::~ScanLineZBuffer(){
    if(z_buffer != nullptr)delete[] z_buffer;
}

void ScanLineZBuffer::Init(int id, std::vector<ScanLineTriangleInfo> &tris){
    scene_id = id;
    // 构建分类多边形和分类边表
    assert(width > 0 && height > 0);
    classified_triangle_table.clear();
    classified_edge_table.clear();
    classified_triangle_table.resize(height);
    classified_edge_table.resize(height);

    this -> tris = tris;

    // std::cout << classified_triangle_table.size();
    int faces = 0;
    for(ScanLineTriangleInfo &tri: tris){
        if(tri.x_min < 0 || tri.y_min < 0 || tri.x_max >= width || tri.y_max >= height)continue;
        faces ++;
        classified_triangle_table[tri.y_max].push_back({tri.id, tri.dy, tri.y_max});
        // std::cout << 111 << std::endl;
        auto edges = tri.GetEdges();
        for(ClassifiedEdge &edge: edges){
            classified_edge_table[edge.y_max].push_back(edge);
        }
    }
    std::cout << faces << std::endl;
}

void ScanLineZBuffer::Scan(unsigned char *framebuffer){
    active_triangle_table.clear();
    active_edge_table.clear();

    // 从上往下扫描
    // std::cout << classified_triangle_table.size() << std::endl;
    int faces = 0;
    for(int i = height - 1; i >= 0; i --){
        // std::cout << 111 << std::endl;
        // 重置zbuffer
        memset(z_buffer, -1, width * sizeof(float));
        // 检查分类多边形表 加入新的多边形
        // std::cout << 111 << std::endl;
        for(auto &classified_triangle: classified_triangle_table[i]){
            // std::cout << 111 << std::endl;
            active_triangle_table.push_back({classified_triangle.id, classified_triangle.dy});
            faces++;
            // 将相关边加入活化边表
            // 找到顶点在此处的两条边
            ClassifiedEdge e1, e2;
            for(int j = 0; j < classified_edge_table[i].size(); j ++){
                if(classified_edge_table[i][j].id == classified_triangle.id){
                    // 两条边是连着存储的
                    e1 = classified_edge_table[i][j], e2 = classified_edge_table[i][j + 1];
                    // std::cout << e1.dy << " " << e2.dy << std::endl;
                    // assert(e1.id == e2.id);
                    break;
                }
            }
            ActiveEdge active_edge = GetActiveEdge(e1, e2);
            active_edge_table.push_back(active_edge);
        }

        // std::cout << 111 << std::endl;

        // 遍历活化边对 进行光栅化
        for(auto &active_edge: active_edge_table){
            // if(active_edge.xl > active_edge.xr)std::cout << active_edge.xl << " " << active_edge.xr << std::endl;
            // 确定左右边顺序正确
            if(active_edge.xl > active_edge.xr){
                std::swap(active_edge.xl, active_edge.xr);
                std::swap(active_edge.dxl, active_edge.dxr);
                std::swap(active_edge.dyl, active_edge.dyr);
                std::swap(active_edge.l_vs_output[0], active_edge.r_vs_output[0]);
                std::swap(active_edge.l_vs_output[1], active_edge.r_vs_output[1]);
            }
            // 插值得到两个边界点的属性
            // float left_factor = (active_edge.l_vs_output[0].screen_position(0) - active_edge.xl) / (active_edge.l_vs_output[0].screen_position(0) - active_edge.l_vs_output[1].screen_position(0));
            // float right_factor = (active_edge.r_vs_output[0].screen_position(0) - active_edge.xr) / (active_edge.r_vs_output[0].screen_position(0) - active_edge.r_vs_output[1].screen_position(0));
            // std::cout << left_factor << " " << right_factor << std::endl;
            float left_factor = 1 - active_edge.dyl / (active_edge.l_vs_output[0].screen_position(1) - active_edge.l_vs_output[1].screen_position(1) + 0.5);
            float right_factor = 1 - active_edge.dyr / (active_edge.r_vs_output[0].screen_position(1) - active_edge.r_vs_output[1].screen_position(1) + 0.5);
            // std::cout << left_factor << " " << right_factor << std::endl;
            // std::cout << std::endl;
            // std::cout << active_edge.xl << " " << active_edge.xr << std::endl;
            // std::cout << active_edge.l_vs_output[0].screen_position(1) << "  " << active_edge.l_vs_output[1].screen_position(1) << std::endl;
            // std::cout << active_edge.l_vs_output[0].screen_position(0) << "  " << active_edge.xl << " " << active_edge.l_vs_output[1].screen_position(0) << std::endl;
            // std::cout << std::endl;
            auto left = VertexShaderOutput::Lerp(active_edge.l_vs_output[0], active_edge.l_vs_output[1], left_factor);
            auto right = VertexShaderOutput::Lerp(active_edge.r_vs_output[0], active_edge.r_vs_output[1], right_factor);
            auto shader = tris[active_edge.id].shader;
            int len = right.screen_position(0) - left.screen_position(0) + 1.5;
            for(int j = 0; j < len; j ++){
                VertexShaderOutput v = VertexShaderOutput::Lerp(left, right, j * 1.f / len);
                int x = v.screen_position(0) - 0.5;
                int y = i;
                // std::cout << y << " " << i << std::endl;
                if(!WriteZBuffer(x, y, v.screen_position(2)))continue;
                Vec3f color = shader -> FragmentShader(v);
                y = height - y - 1;
                // std::cout << x << " " << y << std::endl;
                // std::cout << x << " " << y << std: endl;
                framebuffer[3 * (y * width + x)] = color(0);
                framebuffer[3 * (y * width + x) + 1] = color(1);
                framebuffer[3 * (y * width + x) + 2] = color(2);
            }
        }
        // std::cout << 222 << std::endl;

        if(i == 0)break; // 最后一行 无需更新

        // 更新活化多边形表
        for(auto it = active_triangle_table.begin(); it != active_triangle_table.end(); ){
            auto &active_triangle = *it;
            active_triangle.dy -= 1;
            if(active_triangle.dy < 0){
                // 删除对应活化边
                int id = active_triangle.id;
                for(int i = 0; i < active_edge_table.size(); i++){
                    if(active_edge_table[i].id == id){
                        active_edge_table.erase(active_edge_table.begin() + i);
                        break;
                    }
                }
                // std::cout << 555 << std::endl;
                // 删除活化多边形
                it = active_triangle_table.erase(it);
                continue;
            }
            it++;
        }

        // std::cout << 333 << std::endl;

        // 更新活化边表
        for(auto &active_edge: active_edge_table){
            active_edge.xl += active_edge.dxl, active_edge.xr += active_edge.dxr;
            active_edge.dyl -= 1, active_edge.dyr -= 1;
            // std::cout << active_edge.dyl << " " << active_edge.dyr << std::endl;
            // dz 目前没用到
            if(active_edge.dyl < 0 || active_edge.dyr < 0){
                bool find = false; 
                ClassifiedEdge e;
                for(auto &edge: classified_edge_table[i]){
                    if(edge.id == active_edge.id){
                        find = true;
                        e = edge;
                        break;
                    }
                }
                // std::cout << find << std::endl;
                // assert(find == true);
                UpdateActiveEdge(active_edge, e); 
                // std::cout << active_edge.dyl << " " << active_edge.dyr << std::endl;
                // if(active_edge.xl > active_edge.xr)std::cout << active_edge.xl << " " << active_edge.xr << std::endl;
            }
        }

        // std::cout << 444 << std::endl;
    }
    // std::cout << faces << std::endl;
}

bool ScanLineZBuffer::WriteZBuffer(int x, int y, float depth){
    if(x < 0 || y < 0 || x >= width || y >= height)return false;
    int pos = x;
    if(z_buffer[pos] > depth)return false; // 不需要修改
    z_buffer[pos] = depth;
    return true;
}

ActiveEdge ScanLineZBuffer::GetActiveEdge(ClassifiedEdge e1, ClassifiedEdge e2){
    // if(e1.dx > e2.dx)return GetActiveEdge(e2, e1);
    // 左右在此无所谓 在实际光栅化时会判断左右
    ActiveEdge res;
    int id = e1.id;
    float xl, xr;
    float dxl, dxr;
    int dyl, dyr;
    float zl;
    float dzx, dzy;

    xl = e1.x, xr = e2.x;
    dxl = e1.dx, dxr = e2.dx;
    dyl = e1.dy, dyr = e2.dy;
    zl = e1.vs_output[0].screen_position(2);
    dzx = (e2.vs_output[0].screen_position(2) - e1.vs_output[0].screen_position(2)) / (xr - xl);
    dzy = (e1.vs_output[1].screen_position(2) - e1.vs_output[0].screen_position(2)) / e1.dy;

    // std::cout << xl << " " << dxl << " " << e1.vs_output[0].screen_position(0) << " " << e1.vs_output[1].screen_position(0) << std::endl;

    res = {xl, xr, dxl, dxr, dyl, dyr, zl, dzx, dzy, id};
    res.l_vs_output[0] = e1.vs_output[0], res.l_vs_output[1] = e1.vs_output[1];
    res.r_vs_output[0] = e2.vs_output[0], res.r_vs_output[1] = e2.vs_output[1];

    return res;
}

void ScanLineZBuffer::UpdateActiveEdge(ActiveEdge &active_edge, ClassifiedEdge e){
    float x = e.x;
    float dx = e.dx;
    int dy = e.dy - 1;
    if(active_edge.dyl < 0){
        active_edge.xl = x;
        active_edge.dxl = dx;
        active_edge.dyl = dy;
        active_edge.l_vs_output[0] = e.vs_output[0], active_edge.l_vs_output[1] = e.vs_output[1];
    }else{
        active_edge.xr = x;
        active_edge.dxr = dx;
        active_edge.dyr = dy;
        active_edge.r_vs_output[0] = e.vs_output[0], active_edge.r_vs_output[1] = e.vs_output[1];
    }

    if(active_edge.xl > active_edge.xr){
        std::swap(active_edge.xl, active_edge.xr);
        std::swap(active_edge.dxl, active_edge.dxr);
        std::swap(active_edge.dyl, active_edge.dyr);
        std::swap(active_edge.l_vs_output[0], active_edge.r_vs_output[0]);
        std::swap(active_edge.l_vs_output[1], active_edge.r_vs_output[1]);
    }
}

int ScanLineZBuffer::GetSceneID(){
    return scene_id;
}