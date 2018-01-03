#ifndef SCENE_H
#define SCENE_H

#include "Geometry.h"
#include "Texture.h"
#include <vector>
#include <memory>

struct Light
{
    Vector3 position;
    Vector3 color;
};

struct Scene
{
    Vector3 backgroundColor;
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<Light> lights;
    std::vector<Texture> textures;

    Scene() {}

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
};

#endif