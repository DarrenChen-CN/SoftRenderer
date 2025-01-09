#pragma once
#include "define.hpp"
#include "hizbuffer.hpp"
#include "zbuffer.hpp"
#include "scanlinezbuffer.hpp"
enum HiddenSufaceRemovalType{ZBUFFER, HIZBUFFER, BVHHIZBUFFER, SCANLINE};

class FrameBuffer{
public:
    FrameBuffer();
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    int Width();
    int Height();
    void SetWidth(int width);
    void SetHeight(int height);
    void Init();
    unsigned char *GetFrameBuffer();

    void WriteBuffer(int x, int y, Vec3f color);
    void CleaZBuffer();
    bool WriteZBuffer(int x, int y, float depth); // 返回值表示是否修改zbuffer true为修改

    // hizbuffer 接口
    bool Reject(Bounds2 triangle_bound);

    void SetHSRType(HiddenSufaceRemovalType hsrtype);

    // scanline zbuffer接口
    void ScanLineInit(int id, std::vector<ScanLineTriangleInfo> &tris);
    int ScanLineSceneId();
    void Scan();



private:
    int width = 800, height = 600;
    unsigned char *framebuffer = nullptr;

    HiddenSufaceRemovalType hsrtype = ZBUFFER;

    ZBuffer *zbuffer = nullptr;
    HiZBuffer *hi_zbuffer = nullptr;
    ScanLineZBuffer *scanline_zbuffer = nullptr;
};