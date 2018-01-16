#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"
#include <stdint.h>
#include <memory>

class Matrix4
{
private:
    float data[16];

public:
    Matrix4();
    Matrix4(std::initializer_list<float> list);

    static Matrix4 RotationX(float angle);
    static Matrix4 RotationY(float angle);
    static Matrix4 RotationZ(float angle);
    static Matrix4 Translation(float dx, float dy, float dz);
    static Matrix4 Identity();

    inline void Set(uint32_t i, uint32_t j, float value)
    {
        data[i * 4 + j] = value;
    }

    inline float Get(uint32_t i, uint32_t j) const
    {
        return data[i * 4 + j];
    }

    Vector3 operator*(Vector3 v) const;
    Matrix4 operator*(const Matrix4& other) const;
};

#endif