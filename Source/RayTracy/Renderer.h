#ifndef RENDERER_H
#define RENDERER_H

#include "Scene.h"

class Renderer
{
public:
    Renderer();

    bool Initialize(int argc, char** argv);
    void Render(uint8_t* buffer, uint32_t width, uint32_t height);
    void CleanUp();

    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer& other) = delete;

private:
    Scene scene;
    uint32_t maxDepth;

    Vector3 RestrictColor(Vector3 color) const;
    Vector3 CastRay(Ray ray, uint32_t depth) const;
    Ray GetPrimaryRay(uint32_t width, uint32_t height, uint32_t x, uint32_t y, float fov) const;
    void SetPixel(uint8_t* buffer, uint32_t width, uint32_t x, uint32_t y, Vector3 color) const;
    Vector3 CalculateColor(Material material, Vector3 normal, Ray ray, float distance, float u, float v) const;
    bool CheckIntersection(Ray ray, float maxDistance) const;
    uint8_t ToByte(float value) const;
    Vector3 GetMaterialColor(Material material, float u, float v) const;
};

#endif