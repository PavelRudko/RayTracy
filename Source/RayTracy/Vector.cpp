#include "Vector.h"

Vector2::Vector2(float x, float y) :
    x(x),
    y(y)
{
}

float Vector2::GetLength()
{
    return sqrtf(x * x + y * y);
}

void Vector2::Normalize()
{
    float length = GetLength();
    x /= length;
    y /= length;
}

Vector2 Vector2::operator * (float multiplier)
{
    return Vector2{ x * multiplier, y * multiplier };
}

Vector2 Vector2::operator + (Vector2 other)
{
    return Vector2{ x + other.x, y + other.y };
}

Vector2 Vector2::operator - (Vector2 other)
{
    return Vector2{ x - other.x, y - other.y };
}

bool Vector2::operator==(const Vector2 & other)
{
    return x == other.x && y == other.y;
}

Vector3::Vector3(float x, float y, float z) : 
    x(x), 
    y(y), 
    z(z)
{
}

float Vector3::GetLength()
{
    return sqrtf(x * x + y * y + z * z);
}

void Vector3::Normalize()
{
    float length = GetLength();
    x /= length;
    y /= length;
    z /= length;
}

Vector3 Vector3::operator * (float multiplier)
{
    return Vector3{ x * multiplier, y * multiplier, z * multiplier };
}

Vector3 Vector3::operator + (Vector3 other)
{
    return Vector3{ x + other.x, y + other.y, z + other.z };
}

Vector3 Vector3::operator - (Vector3 other)
{
    return Vector3{ x - other.x, y - other.y, z - other.z };
}

bool Vector3::operator==(const Vector3 & other)
{
    return x == other.x && y == other.y && z == other.z;
}

float Vector4::GetLength()
{
    return sqrtf(x * x + y * y + z * z + w * w);
}

void Vector4::Normalize()
{
    float length = GetLength();
    x /= length;
    y /= length;
    z /= length;
    w /= length;
}

Vector3 Vector4::ToVector3()
{
    return Vector3{ x, y, z };
}

Vector4 Vector4::operator * (float multiplier)
{
    return Vector4{ x * multiplier, y * multiplier, z * multiplier, w * multiplier };
}

Vector4 Vector4::operator + (Vector4 other)
{
    return Vector4{ x + other.x, y + other.y, z + other.z, w + other.w };
}

Vector4 Vector4::operator - (Vector4 other)
{
    return Vector4{ x - other.x, y - other.y, z - other.z, w - other.w };
}

bool Vector4::operator==(const Vector4 & other)
{
    return x == other.x && y == other.y && z == other.z && w == other.w;
}


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