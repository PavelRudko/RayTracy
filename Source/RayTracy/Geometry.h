#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Vector.h"
#include <stdint.h>

#define PI 3.141592653589

struct Ray 
{
    Vector3 origin;
    Vector3 direction;
};

struct Material
{
    float Ka, Kd, Ks, S, textureScale;
    Vector3 color;
    int texture;

    Material() :
        Ka{ 1 },
        Kd{ 0 },
        Ks{ 0 },
        color{Vector3{0, 0, 0}},
        S{ 0 },
        texture{ -1 },
        textureScale{ 1 }
    {
    }
};

struct Object 
{
    Material material;

    Object() {};
    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0, float* u = 0, float* v = 0) = 0;
    virtual ~Object() {};
};

struct Sphere : public Object 
{
    Vector3 center;
    float radius;

    Sphere() : Object()
    {
        center = {};
        radius = 0;
    }

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0, float* u = 0, float* v = 0) override;
};

struct Plane : public Object
{
    Vector3 point, normal;

    Plane() : Object()
    {
        point = {};
        normal = {};
    }

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0, float* u = 0, float* v = 0) override;
};

struct Disk : public Plane
{
    float radius;

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0, float* u = 0, float* v = 0) override;
};

struct Triangle : public Object
{
    Vector3 a, b, c;

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0, float* u = 0, float* v = 0) override;
};

struct Mesh : public Object
{
    Vector3* vertices;
    uint32_t* indices;
    uint32_t indicesCount;
    uint32_t verticesCount;

    Mesh();
    Mesh(Mesh&& other);
    Mesh(const Mesh& other) = delete;

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0, float* u = 0, float* v = 0) override;

    void Resize(uint32_t verticesCount, uint32_t indicesCount);

    inline void SetIndex(uint32_t index, uint32_t value) 
    {
        indices[index] = value;
    }

    inline void SetVertex(uint32_t index, Vector3 value)
    {
        vertices[index] = value;
    }

    ~Mesh();
};



#endif