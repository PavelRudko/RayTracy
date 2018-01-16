#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

struct Vector2
{
    float x;
    float y;

    Vector2(float x = 0, float y = 0);

    float GetLength();
    void Normalize();
    Vector2 operator * (float multiplier);
    Vector2 operator + (Vector2 other);
    Vector2 operator - (Vector2 other);
};

struct Vector3
{
    float x;
    float y;
    float z;

    Vector3(float x = 0, float y = 0, float z = 0);

    float GetLength();
    void Normalize();
    Vector3 operator * (float multiplier);
    Vector3 operator + (Vector3 other);
    Vector3 operator - (Vector3 other);
};

struct Vector4
{
    float x;
    float y;
    float z;
    float w;

    float GetLength();
    void Normalize();
    Vector3 ToVector3();
    Vector4 operator * (float multiplier);
    Vector4 operator + (Vector4 other);
    Vector4 operator - (Vector4 other);
};

float Dot(Vector3 a, Vector3 b);
Vector3 Cross(Vector3 a, Vector3 b);
Vector3 GetOppositeNormal(Vector3 normal, Vector3 direction);

#endif
