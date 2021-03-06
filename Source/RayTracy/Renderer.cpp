#include "Renderer.h"
#include "SceneLoader.h"
#include <iostream>
#include <math.h>
#include <cmath>

Renderer::Renderer() : maxDepth(3), samplesCount(2)
{
}

bool Renderer::Initialize(int argc, char** argv)
{
    if (argc != 2) {
        printf("Specify scene file path.\n");
        return false;
    }

    SceneLoader loader;
    return loader.LoadScene(argv[1], &scene);
}

void Renderer::Render(uint8_t* buffer, uint32_t width, uint32_t height)
{
    uint32_t sampleWidth = width * samplesCount;
    uint32_t sampleHeight = height * samplesCount;
    float averageFactor = (1.0f / (samplesCount * samplesCount));
    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            Vector3 sum { 0, 0, 0 };
            for (uint32_t dx = 0; dx < samplesCount; dx++) {
                for (uint32_t dy = 0; dy < samplesCount; dy++) {
                    auto ray = GetPrimaryRay(sampleWidth, sampleHeight, x * samplesCount + dx, y * samplesCount + dy, PI / 4);
                    sum = sum + CastRay(ray, 0, sampleWidth * sampleHeight);
                }
            }

            SetPixel(buffer, width, x, y, sum * averageFactor);
        }
    }
}

Vector3 Renderer::RestrictColor(Vector3 color) const
{
    if (color.x > 1) {
        color.x = 1;
    }

    if (color.y > 1) {
        color.y = 1;
    }

    if (color.z > 1) {
        color.z = 1;
    }

    return color;
}

Vector3 Renderer::CastRay(Ray ray, uint32_t depth, uint32_t resolution) const
{
    Material material;
    Vector3 normal;
    
    float minDistance = INFINITY, minU, minV;
    bool hasIntersection = false;

    for (auto& object : scene.objects) {
        float distance, u, v;
        Vector3 n;
        if (object->HasIntersection(ray, &distance, &n, &u, &v) && distance < minDistance) {
            hasIntersection = true;
            material = object->material;
            minDistance = distance;
            normal = n;
            minU = u;
            minV = v;
        }
    }

    auto color = hasIntersection ? CalculateColor(material, normal, ray, minDistance, minU, minV, resolution) : scene.backgroundColor;
    if (hasIntersection) {
        float kr = material.reflectivity;
        if (material.ior > 1 && depth < maxDepth) {
            Ray refracted;
            if (Refract(ray.direction, normal, material.ior, &refracted.direction, &kr)) {
                bool outside = Dot(ray.direction, normal) < 0;
                auto bias = normal * 0.0001;
                auto point = (ray.origin + ray.direction * minDistance);
                refracted.origin = outside ? point - bias : point + bias;
                auto refractedColor = CastRay(refracted, depth + 1, resolution);
                color = color + refractedColor * (1 - kr);
            }
        }
        if (kr > 0 && depth < maxDepth) {
            Ray reflected;
            reflected.direction = ray.direction - normal * (2 * Dot(normal, ray.direction));
            reflected.direction.Normalize();
            reflected.origin = (ray.origin + ray.direction * minDistance) + reflected.direction * 0.0001;
            auto reflectedColor = CastRay(reflected, depth + 1, resolution);
            color = color + reflectedColor * kr;
        }
    }
    
    return RestrictColor(color);
}

void Renderer::SetPixel(uint8_t* buffer, uint32_t width, uint32_t x, uint32_t y, Vector3 color) const
{
    uint32_t index = 4 * (y * width + x);

    buffer[index] = ToByte(color.z);
    buffer[index + 1] = ToByte(color.y);
    buffer[index + 2] = ToByte(color.x);
    buffer[index + 3] = 255;
}

Vector4 Renderer::FilterTexture(const Texture& texture, float x, float y, float distance, uint32_t resolution, float textureScale, float mipBias) const
{
    float texelSize = (textureScale / texture.GetWidth()) * (textureScale / texture.GetHeight());
    float factor = texelSize * resolution;
    int level = log2(distance / factor) + mipBias;
    auto currentLevelColor = texture.GetPixel(x, y, std::fmax(0, level));
    if (level <= 0) {
        return currentLevelColor;
    }
    float minDistance = pow(2, level - mipBias) * factor;
    float maxDistance = pow(2, level + 1 - mipBias) * factor;
    float k = (distance - minDistance) / (maxDistance - minDistance);
    auto previousLevelColor = texture.GetPixel(x, y, std::fmax(0, level - 1));
    return currentLevelColor * k + previousLevelColor * (1 - k);
}

Vector3 Renderer::GetMaterialColor(Material material, float u, float v, float distance, uint32_t resolution) const
{
    auto color = material.color;
    if (material.texture >= 0 && material.texture < scene.textures.size()) {
        float texX = u * material.textureScale;
        float texY = v * material.textureScale;
        auto& texture = scene.textures[material.texture];
        auto textureColor = texture.HasMipmap() ? FilterTexture(texture, texX, texY, distance, resolution, material.textureScale, material.mipBias) : texture.GetPixel(texX, texY);
        
        color.x *= textureColor.x;
        color.y *= textureColor.y;
        color.z *= textureColor.z;
    }
    return color;
}

Vector3 Renderer::CalculateColor(Material material, Vector3 normal, Ray ray, float distance, float u, float v, uint32_t resolution) const
{
    auto point = ray.origin + ray.direction * distance;
    auto materialColor = GetMaterialColor(material, u, v, distance, resolution);
    auto color = materialColor * material.Ka;
    for (auto light : scene.lights) {
        auto toLight = light.position - point;
        float distanceToLight = toLight.GetLength();
        toLight.Normalize();

        Ray rayToLight;
        rayToLight.origin = point + toLight * 0.0001;
        rayToLight.direction = toLight;

        if (CheckIntersection(rayToLight, distanceToLight)) {
            continue;
        }

        float d = Dot(toLight, normal) * material.Kd;

        auto reflected = toLight - (normal * 2 * d);
        reflected.Normalize();

        auto toEye = ray.origin - point;
        toEye.Normalize();

        float s = -Dot(reflected, toEye);

        color = color + materialColor * d;
        if (s > 0 && material.Ks > 0) {
            s = pow(s, material.S) * material.Ks;
            color = color + light.color * s;
        }
    }

    return RestrictColor(color);
}

bool Renderer::CheckIntersection(Ray ray, float maxDistance) const
{
    float distance;
    for (auto& object : scene.objects) {
        if (object->HasIntersection(ray, &distance) && distance < maxDistance) {
            return true;
        }
    }
    return false;
}

uint8_t Renderer::ToByte(float value) const
{
    int integer = value * 255;

    if (value > 255) {
        return 255;
    }

    if (value < 0) {
        return 0;
    }

    return integer;
}

Ray Renderer::GetPrimaryRay(uint32_t width, uint32_t height, uint32_t x, uint32_t y, float fov) const
{
    float ratio = (float)width / height;
    float screenX = 2 * (float)x / width - 1;
    float screenY = 1 - 2 * (float)y / height;

    if (width > height) {
        screenX *= ratio;
    }
    else {
        screenY /= ratio;
    }

    float fovCoefficient = tan(fov / 2);

    screenX *= fovCoefficient;
    screenY *= fovCoefficient;

    Ray ray = {};

    ray.direction.x = screenX;
    ray.direction.y = screenY;
    ray.direction.z = -1;

    ray.direction.Normalize();

    return ray;
}

float Clamp(float min, float max, float value)
{
    if (value < min) {
        return min;
    }

    if (value > max) {
        return max;
    }

    return value;
}

bool Renderer::Refract(Vector3 direction, Vector3 normal, float ior, Vector3* refracted, float* kr) const
{
    float cosi = Clamp(-1, 1, Dot(direction, normal));
    float etai = 1, etat = ior;
    if (cosi < 0) {
        cosi = -cosi;
    }
    else {
        normal = normal * (-1);
        std::swap(etai, etat);
    }
    float eta = etai / etat;
    float sint = eta * sqrtf(std::fmax(0.0f, 1 - cosi * cosi));

    if (sint >= 1) {
        return false;
    }

    float cost = sqrtf(std::fmax(0.0f, 1 - sint * sint));
    float rs = (etat * cosi - etai * cost) / (etat * cosi - etai * cost);
    float rp = (etai * cosi - etat * cost) / (etai * cosi + etat * cost);
    
    *kr = (rs * rs + rp * rp) / 2;
    *refracted = direction * eta + normal * (eta * cosi - cost);

    return true;
}

void Renderer::CleanUp()
{
    scene.textures.clear();
    scene.objects.clear();
    scene.lights.clear();
}