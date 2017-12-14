#include "Renderer.h"
#include "SceneLoader.h"
#include <iostream>

bool Renderer::Initialize(int argc, char** argv)
{
    if (argc != 2) {
        printf("Specify scene file path.\n");
        return false;
    }

    SceneLoader loader;
    return loader.LoadScene(argv[1], scene);
}

void Renderer::Render(uint8_t* buffer, uint32_t width, uint32_t height)
{
    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            Material material;
            Vector3 normal;
            auto ray = GetPrimaryRay(width, height, x, y, PI / 4);
            float minDistance = INFINITY;
            bool hasIntersection = false;

            for (auto object : scene.objects) {
                float distance;
                Vector3 n;
                if (object->HasIntersection(ray, &distance, &n) && distance < minDistance) {
                    hasIntersection = true;
                    material = object->material;
                    minDistance = distance;
                    normal = n;
                }
            }

            SetPixel(buffer, width, x, y, hasIntersection ? CalculateColor(material, normal, ray, minDistance) : scene.backgroundColor);
        }
    }
}

void Renderer::SetPixel(uint8_t* buffer, uint32_t width, uint32_t x, uint32_t y, Vector3 color) const
{
    uint32_t index = 4 * (y * width + x);

    buffer[index] = ToByte(color.z);
    buffer[index + 1] = ToByte(color.y);
    buffer[index + 2] = ToByte(color.x);
    buffer[index + 3] = 255;
}

Vector3 Renderer::CalculateColor(Material material, Vector3 normal, Ray ray, float distance) const
{
    auto point = ray.origin + ray.direction * distance;
    auto color = material.color * material.Ka;
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

        color = color + material.color * d;
        if (s > 0 && material.Ks > 0) {
            s = pow(s, material.S) * material.Ks;
            color = color + light.color * s;
        }
    }

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

bool Renderer::CheckIntersection(Ray ray, float maxDistance) const
{
    float distance;
    for (auto object : scene.objects) {
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

void Renderer::CleanUp()
{
    for (auto object : scene.objects) {
        delete object;
    }
    scene.objects.clear();
    scene.lights.clear();
}