// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <cuda_runtime.h>

namespace retinify
{
struct Matrix4x4f
{
    float4 rows[4];
};

__device__ __forceinline__ float DotRow(const Matrix4x4f &matrix, int row, float u, float v, float disparity)
{
    const float4 r = matrix.rows[row];
    return (r.x * u) + (r.y * v) + (r.z * disparity) + r.w;
}

__host__ __device__ __forceinline__ std::uint32_t DivUp(std::uint32_t value, std::uint32_t divisor)
{
    return (value + divisor - 1U) / divisor;
}

inline Matrix4x4f MakeMatrix4x4f(const float *elements)
{
    Matrix4x4f matrix{};
    matrix.rows[0] = make_float4(elements[0], elements[1], elements[2], elements[3]);
    matrix.rows[1] = make_float4(elements[4], elements[5], elements[6], elements[7]);
    matrix.rows[2] = make_float4(elements[8], elements[9], elements[10], elements[11]);
    matrix.rows[3] = make_float4(elements[12], elements[13], elements[14], elements[15]);
    return matrix;
}
} // namespace retinify
