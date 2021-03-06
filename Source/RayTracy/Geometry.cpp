#include "Geometry.h"

#define EPSILON 0.000000000001

bool RayTriangleIntersection(Vector3 a, Vector3 b, Vector3 c, Ray ray, float* t, Vector3* normal, float* outU, float* outV)
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

    if (outU) {
        *outU = u;
    }

    if (outV) {
        *outV = v;
    }

    return true;
}

bool Sphere::HasIntersection(Ray ray, float* t, Vector3* normal, float* u, float* v)
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
    float distance = l < radius ? tca + thc : tca - thc;

    if (t) {
        *t = distance;
    }

    if (normal) {
        auto intersectionPoint = ray.origin + ray.direction * distance;
        auto n = intersectionPoint - center;
        n.Normalize();
        *normal = n;

        if (u) {
            *u = 0.5f + atan2(n.z, n.x) / (PI);
        }

        if (v) {
            *v = 0.5f - asin(n.y) / PI;
        }
    }

    return true;
}

void GetPlaneUV(Vector3 p0, Vector3 p, Vector3 n, float* outU, float* outV)
{
    Vector3 U, V;
    if (n.y == 0) {
        V = Vector3(0, -1, 0);
        U = Cross(n, V);
    }
    else {
        U = Vector3(p0.x + 1, (p0.y * n.y - n.x) / n.y, p0.z) - p0;
        V = Vector3(p0.x, (p0.y * n.y - n.z) / n.y, p0.z + 1) - p0;
    }
    U.Normalize();
    V.Normalize();
    auto L = p - p0;
    float l = L.GetLength();
    float t = (L - U).GetLength();
    float m = (L - V).GetLength();
    *outU = (l * l - t * t + 1) / 2;
    *outV = (l * l - m * m + 1) / 2;
}

bool Plane::HasIntersection(Ray ray, float* t, Vector3* intersectionNormal, float* u, float* v)
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

    if (u && v) {
        auto intersection = ray.direction * distance + ray.origin;
        GetPlaneUV(point, intersection, n, u, v);
    }

    return true;
}

bool Disk::HasIntersection(Ray ray, float* t, Vector3* intersectionNormal, float* u, float* v)
{
    float distanceToPlane;

    if (!Plane::HasIntersection(ray, &distanceToPlane, intersectionNormal, u, v)) {
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

bool Triangle::HasIntersection(Ray ray, float* t, Vector3* normal, float* u, float* v)
{
    return RayTriangleIntersection(a, b, c, ray, t, normal, u, v);
}

Mesh::Mesh() : 
    verticesCount{ 0 }, 
    indicesCount{ 0 }, 
    indices{nullptr}, 
    vertices{nullptr},
    textureCoordinates{nullptr}
{
}

Mesh::Mesh(Mesh&& other)
{
    vertices = other.vertices;
    indices = other.indices;
    textureCoordinates = other.textureCoordinates;
    verticesCount = other.verticesCount;
    indicesCount = other.indicesCount;

    other.vertices = nullptr;
    other.indices = nullptr;
    other.textureCoordinates = nullptr;
}

bool Mesh::HasIntersection(Ray ray, float* t, Vector3* normal, float* u, float* v)
{
    bool hasIntersection = false;
    float minDistance = INFINITY, distance, minU, minV, cu, cv;
    Vector3 minN, n;

    ray.origin = scaleMatrix * rotationMatrix * translationMatrix * ray.origin;
    ray.direction = scaleMatrix * rotationMatrix * ray.direction;

    for (uint32_t i = 0; i < indicesCount; i += 3) {
        uint32_t indexA = indices[i], indexB = indices[i + 1], indexC = indices[i + 2];

        if (indexA >= verticesCount || indexB >= verticesCount || indexC >= verticesCount) {
            continue;
        }

        auto a = vertices[indexA];
        auto b = vertices[indexB];
        auto c = vertices[indexC];

        if (RayTriangleIntersection(a, b, c, ray, &distance, &n, &cu, &cv)) {
            hasIntersection = true;
            if (distance < minDistance) {
                minDistance = distance;
                minN = n;
                if (textureCoordinates) {
                    auto t1 = textureCoordinates[indexA];
                    auto t2 = textureCoordinates[indexB];
                    auto t3 = textureCoordinates[indexC];
                    auto t = t1 * (1 - cu - cv) + t2 * cu + t3 * cv;
                    minU = t.x;
                    minV = 1 - t.y;
                }
                else {
                    minU = cu;
                    minV = cv;
                }
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

    if (u) {
        *u = minU;
    }

    if (v) {
        *v = minV;
    }

    return true;
}

void Mesh::Resize(uint32_t verticesCount, uint32_t indicesCount, bool hasTextureCoordinates)
{
    if (vertices) {
        delete[] vertices;
    }
    if (indices) {
        delete[] indices;
    }
    if (textureCoordinates) {
        delete[] textureCoordinates;
    }
    this->verticesCount = verticesCount;
    this->indicesCount = indicesCount;
    vertices = new Vector3[verticesCount];
    indices = new uint32_t[indicesCount];
    if (hasTextureCoordinates) {
        textureCoordinates = new Vector2[verticesCount];
    }
}

void Mesh::SetTransformation(Vector3 position, Vector3 rotation, float scale)
{
    rotationMatrix = Matrix4::RotationX(-rotation.x) * Matrix4::RotationZ(-rotation.z) * Matrix4::RotationY(-rotation.y);
    translationMatrix = Matrix4::Translation(-position.x, -position.y, -position.z);
    scaleMatrix = Matrix4::Scale(1.0f / scale, 1.0f / scale, 1.0f / scale);
}

Mesh::~Mesh()
{
    if (vertices) {
        delete[] vertices;
    }
    if (indices) {
        delete[] indices;
    }
    if (textureCoordinates) {
        delete[] textureCoordinates;
    }
}
