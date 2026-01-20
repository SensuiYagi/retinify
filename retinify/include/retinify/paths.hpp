// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "retinify/attributes.hpp"
#include "retinify/status.hpp"

#include <filesystem>

namespace retinify
{
/// @brief
/// Returns the current user’s home directory path
/// @param path
/// Output home directory path
/// @return
/// A Status object that indicates whether the operation was successful
RETINIFY_API [[nodiscard]] auto HomeDirectoryPath(std::filesystem::path &path) noexcept -> Status;

/// @brief
/// Returns the configuration directory path for retinify
/// @param path
/// Output configuration directory path
/// @return
/// A Status object that indicates whether the operation was successful
RETINIFY_API [[nodiscard]] auto ConfigDirectoryPath(std::filesystem::path &path) noexcept -> Status;

/// @brief
/// Returns the cache directory path for retinify
/// @param path
/// Output cache directory path
/// @return
/// A Status object that indicates whether the operation was successful
RETINIFY_API [[nodiscard]] auto CacheDirectoryPath(std::filesystem::path &path) noexcept -> Status;

/// @brief
/// Returns the data directory path for retinify
/// @param path
/// Output data directory path
/// @return
/// A Status object that indicates whether the operation was successful
RETINIFY_API [[nodiscard]] auto DataDirectoryPath(std::filesystem::path &path) noexcept -> Status;

/// @brief
/// Returns the state directory path for retinify
/// @param path
/// Output state directory path
/// @return
/// A Status object that indicates whether the operation was successful
RETINIFY_API [[nodiscard]] auto StateDirectoryPath(std::filesystem::path &path) noexcept -> Status;
} // namespace retinify
