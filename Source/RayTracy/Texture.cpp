#include "Texture.h"
#include <utility>
#include <math.h>

Texture::Texture(uint32_t width, uint32_t height, uint32_t bytesPerPixel, bool mipmap) :
    width(width),
    height(height),
    bytesPerPixel(bytesPerPixel),
    mipmap(mipmap),
    data(nullptr)
{
    if (width == 0 || height == 0) {
        return;
    }
    uint32_t size = width * height * bytesPerPixel;
    if (mipmap) {
        size += ceil(size / 3.0);
    }
    data = new uint8_t[size];
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
    mipmap = other.mipmap;
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

Vector4 Texture::GetPixel(uint32_t x, uint32_t y, uint32_t level) const
{
    if (!mipmap || level == 0) {
        return GetPixel((y * width + x) * bytesPerPixel);
    }
    uint32_t offset = 0;
    uint32_t levelWidth = width, levelHeight = height;
    while (levelWidth >= 2 && levelHeight >= 2 && level > 0) {
        offset += levelWidth * levelHeight * bytesPerPixel;
        levelWidth /= 2;
        levelHeight /= 2;
        level--;
        x /= 2;
        y /= 2;
    }
    return GetPixel(offset + (y * levelWidth + x) * bytesPerPixel);
}

Vector4 Texture::GetPixel(float u, float v, uint32_t level) const
{
    uint32_t x = GetCoordinate(u, width);
    uint32_t y = GetCoordinate(v, height);
    return GetPixel(x, y, level);
}

uint8_t* Texture::GetData() const
{
    return data;
}

Vector4 Texture::Interpolate(uint32_t offset, uint32_t x, uint32_t y, uint32_t previousWidth) const
{
    Vector4 sum{ 0, 0, 0 , 0 };
    for (uint32_t dx = 0; dx < 2; dx++) {
        for (uint32_t dy = 0; dy < 2; dy++) {
            uint32_t index = offset + (previousWidth * (y * 2 + dy) + (x * 2 + dx)) * bytesPerPixel;
            Vector4 color {
                Normalize(data[index]),
                Normalize(data[index + 1]),
                Normalize(data[index + 2])
            };
            color.w = bytesPerPixel == 4 ? Normalize(data[index + 3]) : 1;
            sum = sum + color;
        }
    }
    return sum * 0.25;
}

Vector4 Texture::GetPixel(uint32_t index) const
{
    Vector4 color{
        Normalize(data[index]),
        Normalize(data[index + 1]),
        Normalize(data[index + 2])
    };
    color.w = bytesPerPixel == 4 ? Normalize(data[index + 3]) : 1;
    return color;
}

bool Texture::GenerateMipmap()
{
    if (!mipmap) {
        return false;
    }
    uint32_t levelWidth = width, levelHeight = height, previousWidth = width;
    uint32_t previousOffset = 0, levelOffset = 0;
    while (levelWidth >= 2 && levelHeight >= 2) {
        levelOffset += levelWidth * levelHeight * bytesPerPixel;
        levelWidth /= 2;
        levelHeight /= 2;

        uint32_t index = levelOffset;
        for (uint32_t y = 0; y < levelHeight; y++) {
            for (uint32_t x = 0; x < levelWidth; x++) {
                auto color = Interpolate(previousOffset, x, y, previousWidth);
                data[index] = color.x * 255;
                data[index + 1] = color.y * 255;
                data[index + 2] = color.z * 255;
                if (bytesPerPixel == 4) {
                    data[index + 3] = color.w * 255;
                }
                index += bytesPerPixel;
            }
        }

        previousWidth = levelWidth;
    }

    return true;
}

bool Texture::HasMipmap() const
{
    return mipmap;
}

uint32_t Texture::GetWidth() const
{
    return width;
}

uint32_t Texture::GetHeight() const
{
    return height;
}

Texture::~Texture()
{
    if (data) {
        delete[] data;
    }
}
