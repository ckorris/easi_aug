#pragma once

#ifndef VECTORS_H
#define VECTORS_H

#include <iostream>

namespace hps
{
    struct int2 
    {
        int x, y;

        int2() : x(0), y(0) {}

        int2(int x, int y) : x(x), y(y) {}

        int2 operator+(const int2& other) const 
        {
            return int2(x + other.x, y + other.y);
        }

        void print() const 
        {
            std::cout << "int2(" << x << ", " << y << ")" << std::endl;
        }
    };

    struct float3 
    {
        float x, y, z;

        float3() : x(0), y(0), z(0) {}

        float3(float x, float y, float z) : x(x), y(y), z(z) {}

        float3 operator+(const float3& other) const 
        {
            return float3(x + other.x, y + other.y, z + other.z);
        }

        float3 operator*(const float& other) const
        {
            return float3(x * other, y * other, z * other);
        }

        float3 operator*(const float3& other) const 
        {
            return float3(x * other.x, y * other.y, z * other.z);
        }

        float3 operator/(const float& other) const
        {
            return float3(x / other, y / other, z / other);
        }

        float3 operator/(const float3& other) const
        {
            return float3(x / other.x, y / other.y, z / other.z);
        }

        float3& operator+=(const float3& other) 
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        float3& operator-=(const float3& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        void print() const {
            std::cout << "float3(" << x << ", " << y << ", " << z << ")" << std::endl;
        }
    };

} 

#endif //VECTORS_H