// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace retinify
{
/// @brief
/// Base class that disables copy and move operations for derived classes.
class NoCopyMove
{
  protected:
    NoCopyMove() = default;
    ~NoCopyMove() = default;
    NoCopyMove(const NoCopyMove &) = delete;
    auto operator=(const NoCopyMove &) -> NoCopyMove & = delete;
    NoCopyMove(NoCopyMove &&) = delete;
    auto operator=(NoCopyMove &&) -> NoCopyMove & = delete;
};
} // namespace retinify
