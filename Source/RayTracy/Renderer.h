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

    Vector4 Renderer::FilterTexture(const Texture& texture, float x, float y, float distance, uint32_t resolution, float textureScale, float mipBias) const;
    Vector3 RestrictColor(Vector3 color) const;
    bool Refract(Vector3 direction, Vector3 normal, float ior, Vector3* refracted, float* kr) const;
    Vector3 CastRay(Ray ray, uint32_t depth, uint32_t screenWidth) const;
    Ray GetPrimaryRay(uint32_t width, uint32_t height, uint32_t x, uint32_t y, float fov) const;
    void SetPixel(uint8_t* buffer, uint32_t width, uint32_t x, uint32_t y, Vector3 color) const;
    Vector3 CalculateColor(Material material, Vector3 normal, Ray ray, float distance, float u, float v, uint32_t screenWidth) const;
    bool CheckIntersection(Ray ray, float maxDistance) const;
    uint8_t ToByte(float value) const;
    Vector3 Renderer::GetMaterialColor(Material material, float u, float v, float distance, uint32_t screenWidth) const;
};

#endif