#include "Texture.h"
#include <utility>

Texture::Texture(uint32_t width, uint32_t height, uint32_t bytesPerPixel) :
    width(width),
    height(height),
    bytesPerPixel(bytesPerPixel)
{
    data = new uint8_t[width * height * bytesPerPixel];
}

Texture::Texture(Texture&& other)
{
    *this = std::move(other);
}

Texture& Texture::operator=(Texture && other)
{
    data = other.data;
    width = other.width;
    height = other.height;
    bytesPerPixel = other.bytesPerPixel;
    other.data = nullptr;
    return *this;
}

void Texture::SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t index = (y * width + x) * bytesPerPixel;
    data[index] = r;
    data[index + 1] = g;
    data[index + 2] = b;
}

void Texture::SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint32_t index = (y * width + x) * bytesPerPixel;
    data[index] = r;
    data[index + 1] = g;
    data[index + 2] = b;
    if (bytesPerPixel > 3) {
        data[index + 3] = a;
    }
}

Vector4 Texture::GetPixel(uint32_t x, uint32_t y) const
{
    uint32_t index = (y * width + x) * bytesPerPixel;
    Vector4 color { 
        Normalize(data[index]), 
        Normalize(data[index + 1]),
        Normalize(data[index + 2])
    };
    color.w = bytesPerPixel == 4 ? Normalize(data[index + 3]) : 1;
    return color;
}

uint32_t Texture::GetCoordinate(float value, uint32_t range) const
{
    value -= (int)value;
    if (value < 0) {
        value += 1;
    }
    return range * value;
}

float Texture::Normalize(uint8_t value) const
{
    float result = value / 255.0f;
    if (result < 0) {
        return 0;
    }
    if (result > 1) {
        return 1;
    }
    return result;
}

Vector4 Texture::GetPixel(float u, float v) const
{
    uint32_t x = GetCoordinate(u, width);
    uint32_t y = GetCoordinate(v, height);
    return GetPixel(x, y);
}

Texture::~Texture()
{
    if (data) {
        delete[] data;
    }
}
