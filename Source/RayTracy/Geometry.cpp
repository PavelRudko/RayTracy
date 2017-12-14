#include "Geometry.h"

#define EPSILON 0.000000000001

float Dot(Vector3 a, Vector3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Cross(Vector3 a, Vector3 b)
{
    return Vector3
    {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vector3 GetOppositeNormal(Vector3 normal, Vector3 direction)
{
    float angle = Dot(normal, direction);
    return angle > 0 ? normal * -1 : normal;
}

bool RayTriangleIntersection(Vector3 a, Vector3 b, Vector3 c, Ray ray, float* t, Vector3* normal)
{
    auto ab = b - a;
    auto ac = c - a;

    auto dac = Cross(ray.direction, ac);
    float det = Dot(dac, ab);

    if (fabs(det) < EPSILON) {
        return false;
    }

    auto T = ray.origin - a;

    float u = Dot(dac, T) / det;
    if (u < 0 || u > 1) {
        return false;
    }

    auto tab = Cross(T, ab);
    float v = Dot(tab, ray.direction) / det;
    if (v < 0 || v + u > 1) {
        return false;
    }

    float distance = Dot(tab, ac) / det;
    if (distance < 0) {
        return false;
    }

    if (t) {
        *t = distance;
    }

    if (normal) {
        auto n = Cross(ab, ac);
        n.Normalize();
        *normal = GetOppositeNormal(n, ray.direction);
    }

    return true;
}

bool Sphere::HasIntersection(Ray ray, float* t, Vector3* normal)
{
    Vector3 L = center - ray.origin;
    float tca = Dot(L, ray.direction);

    if (tca < 0) {
        return false;
    }

    float l = L.GetLength();
    float d = sqrtf(l * l - tca * tca);

    if (d < 0 || d > radius) {
        return false;
    }

    float thc = sqrtf(radius * radius - d * d);
    float distance = tca - thc;

    if (t) {
        *t = distance;
    }

    if (normal) {
        auto intersectionPoint = ray.origin + ray.direction * distance;
        auto n = intersectionPoint - center;
        n.Normalize();
        *normal = n;
    }

    return true;
}

bool Plane::HasIntersection(Ray ray, float* t, Vector3* intersectionNormal)
{
    float d = Dot(ray.direction, normal);

    if (fabs(d) < EPSILON) {
        return false;
    }

    Vector3 n = d > 0 ? normal * -1 : normal;
    Vector3 direction = point - ray.origin;
    float distance = Dot(d > 0 ? direction * -1 : direction, n) / d;

    if (distance < 0) {
        return false;
    }

    if (t) {
        *t = distance;
    }

    if (intersectionNormal) {
        *intersectionNormal = n;
    }

    return true;
}

bool Disk::HasIntersection(Ray ray, float* t, Vector3* intersectionNormal)
{
    float distanceToPlane;

    if (!Plane::HasIntersection(ray, &distanceToPlane, intersectionNormal)) {
        return false;
    }

    Vector3 intersection = ray.origin + ray.direction * distanceToPlane;
    if ((point - intersection).GetLength() > radius) {
        return false;
    }

    if (t) {
        *t = distanceToPlane;
    }

    return true;
}

bool Triangle::HasIntersection(Ray ray, float* t, Vector3* normal)
{
    return RayTriangleIntersection(a, b, c, ray, t, normal);
}

Mesh::Mesh(uint32_t verticesCount, uint32_t indicesCount) :
    verticesCount{verticesCount},
    indicesCount{indicesCount}
{
    vertices = new Vector3[verticesCount];
    indices = new uint32_t[indicesCount];
}

Mesh::Mesh(Mesh&& other)
{
    vertices = other.vertices;
    indices = other.indices;
    verticesCount = other.verticesCount;
    indicesCount = other.indicesCount;

    other.vertices = nullptr;
    other.indices = nullptr;
}

bool Mesh::HasIntersection(Ray ray, float* t, Vector3* normal)
{
    bool hasIntersection = false;
    float minDistance = INFINITY, distance;
    Vector3 minN, n;

    for (uint32_t i = 0; i < indicesCount; i += 3) {
        uint32_t indexA = indices[i], indexB = indices[i + 1], indexC = indices[i + 2];

        auto a = vertices[indexA];
        auto b = vertices[indexB];
        auto c = vertices[indexC];

        if (RayTriangleIntersection(a, b, c, ray, &distance, &n)) {
            hasIntersection = true;
            if (distance < minDistance) {
                minDistance = distance;
                minN = n;
            }
        }
    }

    if (!hasIntersection) {
        return false;
    }

    if (t) {
        *t = minDistance;
    }

    if (normal) {
        *normal = minN;
    }

    return true;
}

Mesh::~Mesh()
{
    if (vertices) {
        delete[] vertices;
    }
    if (indices) {
        delete[] indices;
    }
}
