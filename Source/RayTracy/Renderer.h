#ifndef RENDERER_H
#define RENDERER_H

#include "Geometry.h"

class Renderer
{
public:
    bool Initialize(int argc, char** argv);
    void Render(uint8_t* buffer, uint32_t width, uint32_t height);
    void CleanUp();

private:
    Scene scene;

    Ray GetPrimaryRay(uint32_t width, uint32_t height, uint32_t x, uint32_t y, float fov) const;
    void SetPixel(uint8_t* buffer, uint32_t width, uint32_t x, uint32_t y, Vector3 color) const;
    Vector3 CalculateColor(Material material, Vector3 normal, Ray ray, float distance) const;
    bool CheckIntersection(Ray ray, float maxDistance) const;
    uint8_t ToByte(float value) const;
};

#endif