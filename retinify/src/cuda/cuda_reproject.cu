// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cuda_common.cuh"
#include "cuda_reproject.cuh"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cuda_runtime.h>

namespace retinify
{
__global__ void ReprojectTo3dKernel(const float *__restrict__ disparity, std::size_t disparityStride, float *__restrict__ points3d, std::size_t points3dStride, //
                                    std::uint32_t width, std::uint32_t height, Matrix4x4f reprojectionMatrix)
{
    const std::uint32_t x = static_cast<std::uint32_t>(blockIdx.x) * static_cast<std::uint32_t>(blockDim.x) + static_cast<std::uint32_t>(threadIdx.x);
    const std::uint32_t y = static_cast<std::uint32_t>(blockIdx.y) * static_cast<std::uint32_t>(blockDim.y) + static_cast<std::uint32_t>(threadIdx.y);

    if (x >= width || y >= height)
    {
        return;
    }

    const std::size_t disparityRowOffset = (static_cast<std::size_t>(y) * disparityStride) / sizeof(float);
    const std::size_t points3dRowOffset = (static_cast<std::size_t>(y) * points3dStride) / sizeof(float);

    const float *disparityRow = disparity + disparityRowOffset;
    float *pointRow = points3d + points3dRowOffset;

    float outX = 0.0f;
    float outY = 0.0f;
    float outZ = 0.0f;

    const float disparityValue = disparityRow[x];
    if (disparityValue > 0.0f && isfinite(disparityValue))
    {
        const float u = static_cast<float>(x);
        const float v = static_cast<float>(y);

        const float X = DotRow(reprojectionMatrix, 0, u, v, disparityValue);
        const float Y = DotRow(reprojectionMatrix, 1, u, v, disparityValue);
        const float Z = DotRow(reprojectionMatrix, 2, u, v, disparityValue);
        const float W = DotRow(reprojectionMatrix, 3, u, v, disparityValue);

        if (fabsf(W) > 1e-6f)
        {
            const float invW = 1.0f / W;
            outX = X * invW;
            outY = Y * invW;
            outZ = Z * invW;
        }
    }

    const std::size_t idx = static_cast<std::size_t>(x) * 3;
    pointRow[idx + 0] = outX;
    pointRow[idx + 1] = outY;
    pointRow[idx + 2] = outZ;
}

cudaError_t cudaReprojectTo3d(const float *disparity, std::size_t disparityStride, float *points3d, std::size_t points3dStride, //
                              std::uint32_t width, std::uint32_t height, const float *reprojectionMatrix, cudaStream_t stream)
{
    if (disparity == nullptr || points3d == nullptr || reprojectionMatrix == nullptr)
    {
        return cudaErrorInvalidValue;
    }

    if (width == 0U || height == 0U)
    {
        return cudaErrorInvalidValue;
    }

    if ((disparityStride % sizeof(float)) != 0U || (points3dStride % sizeof(float)) != 0U)
    {
        return cudaErrorInvalidValue;
    }

    const std::size_t requiredDisparityStride = static_cast<std::size_t>(width) * sizeof(float);
    if (disparityStride < requiredDisparityStride)
    {
        return cudaErrorInvalidValue;
    }

    const std::size_t requiredPointsStride = static_cast<std::size_t>(width) * 3U * sizeof(float);
    if (points3dStride < requiredPointsStride)
    {
        return cudaErrorInvalidValue;
    }

    const Matrix4x4f matrix = MakeMatrix4x4f(reprojectionMatrix);

    dim3 block(kBlockW, kBlockH, 1);
    dim3 grid(DivUp(width, static_cast<std::uint32_t>(block.x)), DivUp(height, static_cast<std::uint32_t>(block.y)), 1);

    ReprojectTo3dKernel<<<grid, block, 0, stream>>>(disparity, disparityStride, points3d, points3dStride, width, height, matrix);

    return cudaGetLastError();
}
} // namespace retinify
