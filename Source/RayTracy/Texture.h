#ifndef TEXTURE_H
#define TEXTURE_H

#include "Vector.h"
#include <stdint.h>

class Texture
{
private:
    uint8_t* data;
    uint32_t width, height, bytesPerPixel;
    uint32_t GetCoordinate(float value, uint32_t range) const;
    float Normalize(uint8_t value) const;

public:
    Texture(uint32_t width, uint32_t height, uint32_t bytesPerPixel);
    Texture(Texture&& other);
    Texture& operator = (Texture&& other);

    Texture(const Texture& other) = delete;
    Texture& operator = (const Texture& other) = delete;

    void SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
    void SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    Vector4 GetPixel(uint32_t x, uint32_t y) const;
    Vector4 GetPixel(float u, float v) const;

    ~Texture();
};

#endif
