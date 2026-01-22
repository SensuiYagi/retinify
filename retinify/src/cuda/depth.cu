// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "common.h"
#include "depth.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cuda_runtime.h>

namespace retinify
{
__global__ void DisparityToDepthKernel(const float *__restrict__ disparity, std::size_t disparityStride, float *__restrict__ depth, std::size_t depthStride, //
                                       std::uint32_t width, std::uint32_t height, Matrix4x4f reprojectionMatrix)
{
    const std::uint32_t x = static_cast<std::uint32_t>(blockIdx.x) * static_cast<std::uint32_t>(blockDim.x) + static_cast<std::uint32_t>(threadIdx.x);
    const std::uint32_t y = static_cast<std::uint32_t>(blockIdx.y) * static_cast<std::uint32_t>(blockDim.y) + static_cast<std::uint32_t>(threadIdx.y);

    if (x >= width || y >= height)
    {
        return;
    }

    const std::size_t disparityRowOffset = (static_cast<std::size_t>(y) * disparityStride) / sizeof(float);
    const std::size_t depthRowOffset = (static_cast<std::size_t>(y) * depthStride) / sizeof(float);

    const float *disparityRow = disparity + disparityRowOffset;
    float *depthRow = depth + depthRowOffset;

    float outDepth = 0.0f;
    const float disparityValue = disparityRow[x];

    if (disparityValue > 0.0f && isfinite(disparityValue))
    {
        const float u = static_cast<float>(x);
        const float v = static_cast<float>(y);

        const float Z = DotRow(reprojectionMatrix, 2, u, v, disparityValue);
        const float W = DotRow(reprojectionMatrix, 3, u, v, disparityValue);

        if (fabsf(W) > 1e-6f)
        {
            outDepth = Z / W;
        }
    }

    depthRow[x] = outDepth;
}

cudaError_t cudaDisparityToDepth(const float *disparity, std::size_t disparityStride, float *depth, std::size_t depthStride, //
                                 std::uint32_t width, std::uint32_t height, const float *reprojectionMatrix, cudaStream_t stream)
{
    if (disparity == nullptr || depth == nullptr || reprojectionMatrix == nullptr)
    {
        return cudaErrorInvalidValue;
    }

    if (width == 0U || height == 0U)
    {
        return cudaErrorInvalidValue;
    }

    if ((disparityStride % sizeof(float)) != 0U || (depthStride % sizeof(float)) != 0U)
    {
        return cudaErrorInvalidValue;
    }

    const std::size_t requiredDisparityStride = static_cast<std::size_t>(width) * sizeof(float);
    if (disparityStride < requiredDisparityStride)
    {
        return cudaErrorInvalidValue;
    }

    const std::size_t requiredDepthStride = static_cast<std::size_t>(width) * sizeof(float);
    if (depthStride < requiredDepthStride)
    {
        return cudaErrorInvalidValue;
    }

    const Matrix4x4f matrix = MakeMatrix4x4f(reprojectionMatrix);

    dim3 block(kBlockW, kBlockH, 1);
    dim3 grid(DivUp(width, static_cast<std::uint32_t>(block.x)), DivUp(height, static_cast<std::uint32_t>(block.y)), 1);

    DisparityToDepthKernel<<<grid, block, 0, stream>>>(disparity, disparityStride, depth, depthStride, width, height, matrix);

    return cudaGetLastError();
}
} // namespace retinify
