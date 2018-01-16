#ifndef TEXTURE_H
#define TEXTURE_H

#include "Vector.h"
#include <stdint.h>

class Texture
{
private:
    uint8_t* data;
    uint32_t width, height, bytesPerPixel;
    bool mipmap;
    uint32_t GetCoordinate(float value, uint32_t range) const;
    float Normalize(uint8_t value) const;
    Vector4 Interpolate(uint32_t offset, uint32_t x, uint32_t y, uint32_t previousWidth) const;
    Vector4 GetPixel(uint32_t index) const;

public:
    Texture(uint32_t width, uint32_t height, uint32_t bytesPerPixel, bool mipmap = false);
    Texture(Texture&& other);
    Texture& operator = (Texture&& other);

    Texture(const Texture& other) = delete;
    Texture& operator = (const Texture& other) = delete;

    void SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
    void SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    Vector4 GetPixel(uint32_t x, uint32_t y, uint32_t level = 0) const;
    Vector4 GetPixel(float u, float v, uint32_t level = 0) const;

    uint8_t* GetData() const;

    bool GenerateMipmap();
    bool HasMipmap() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

    ~Texture();
};

#endif
