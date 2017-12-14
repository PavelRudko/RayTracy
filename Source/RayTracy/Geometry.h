#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>
#include <stdint.h>
#include <vector>

#define PI 3.141592653589

struct Vector3 
{
    float x;
    float y;
    float z;

    float GetLength()
    {
        return sqrtf(x * x + y * y + z * z);
    }

    void Normalize()
    {
        float length = GetLength();
        x /= length;
        y /= length;
        z /= length;
    }

    Vector3 operator * (float multiplier)
    {
        return Vector3 { x * multiplier, y * multiplier, z * multiplier };
    }

    Vector3 operator + (Vector3 other)
    {
        return Vector3 { x + other.x, y + other.y, z + other.z };
    }

    Vector3 operator - (Vector3 other)
    {
        return Vector3 { x - other.x, y - other.y, z - other.z };
    }
};

float Dot(Vector3 a, Vector3 b);
Vector3 Cross(Vector3 a, Vector3 b);

struct Ray 
{
    Vector3 origin;
    Vector3 direction;
};

struct Material
{
    float Ka, Kd, Ks, S;
    Vector3 color;

    Material() :
        Ka{ 1 },
        Kd{ 0 },
        Ks{ 0 },
        color{Vector3{0, 0, 0}},
        S{ 0 }
    {
    }
};

struct Light
{
    Vector3 position;
    Vector3 color;
};

struct Object 
{
    Material material;

    Object() {};
    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0) = 0;
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

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0) override;
};

struct Plane : public Object
{
    Vector3 point, normal;

    Plane() : Object()
    {
        point = {};
        normal = {};
    }

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0) override;
};

struct Disk : public Plane
{
    float radius;

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0) override;
};

struct Triangle : public Object
{
    Vector3 a, b, c;

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0) override;
};

struct Mesh : public Object
{
    Vector3* vertices;
    uint32_t* indices;
    uint32_t indicesCount;
    uint32_t verticesCount;

    Mesh(uint32_t verticesCount, uint32_t indicesCount);
    Mesh(Mesh&& other);
    Mesh(const Mesh& other) = delete;

    virtual bool HasIntersection(Ray ray, float* t = 0, Vector3* normal = 0) override;

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

struct Scene
{
    Vector3 backgroundColor;
    std::vector<Object*> objects;
    std::vector<Light> lights;
};

#endif