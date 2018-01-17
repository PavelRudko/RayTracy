#include "Matrix.h"
#include <memory.h>

Matrix4::Matrix4()
{
    memset(data, 0, sizeof(data));
}

Matrix4::Matrix4(std::initializer_list<float> list)
{
    std::copy(list.begin(), list.end(), data);
}

Matrix4 Matrix4::Scale(float sx, float sy, float sz)
{
    return Matrix4
    {
        sx, 0, 0, 0,
        0, sy, 0, 0,
        0, 0, sz, 0,
        0, 0, 0, 1
    };
}

Matrix4 Matrix4::RotationX(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    return Matrix4
    {
        1, 0, 0, 0,
        0, c, -s, 0,
        0, s, c, 0,
        0, 0, 0, 1
    };
}

Matrix4 Matrix4::RotationY(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    return Matrix4
    {
        c, 0, s, 0,
        0, 1, 0, 0,
        -s, 0, c, 0,
        0, 0, 0, 1
    };
}

Matrix4 Matrix4::RotationZ(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    return Matrix4
    {
        c, -s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

Matrix4 Matrix4::Translation(float dx, float dy, float dz)
{
    return Matrix4
    {
        1, 0, 0, dx,
        0, 1, 0, dy,
        0, 0, 1, dz,
        0, 0, 0, 1
    };
}

Matrix4 Matrix4::Identity()
{
    return Matrix4
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

Matrix4 Matrix4::operator*(const Matrix4& other) const
{
    Matrix4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sum = 0;
            for (int k = 0; k < 4; k++) {
                sum += Get(i, k) * other.Get(k, j);
            }
            result.Set(i, j, sum);
        }
    }
    return result;
}

Vector3 Matrix4::operator*(Vector3 v) const
{
    return Vector3
    {
        Get(0, 0) * v.x + Get(0, 1) * v.y + Get(0, 2) * v.z + Get(0, 3),
        Get(1, 0) * v.x + Get(1, 1) * v.y + Get(1, 2) * v.z + Get(1, 3),
        Get(2, 0) * v.x + Get(2, 1) * v.y + Get(2, 2) * v.z + Get(2, 3)
    };
}