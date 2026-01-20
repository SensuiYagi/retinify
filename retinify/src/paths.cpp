// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "retinify/paths.hpp"
#include "retinify/logging.hpp"
#include "retinify/retinifyModels.hpp"
#include "retinify/version.hpp"

#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>

namespace retinify
{
namespace
{
struct DirectoryInfo
{
    const char *environmentVariable;
    const char *fallbackRelativeToHome;
};

constexpr DirectoryInfo kConfigDirectoryInfo{"XDG_CONFIG_HOME", ".config"};
constexpr DirectoryInfo kCacheDirectoryInfo{"XDG_CACHE_HOME", ".cache"};
constexpr DirectoryInfo kDataDirectoryInfo{"XDG_DATA_HOME", ".local/share"};
constexpr DirectoryInfo kStateDirectoryInfo{"XDG_STATE_HOME", ".local/state"};

constexpr const char *kRetinifyDirName = "retinify";

static inline auto GetUserDirectoryPath(const DirectoryInfo &info, std::filesystem::path &path) noexcept -> Status
{
    try
    {
        std::filesystem::path baseDirectory;

        const char *xdgValue = std::getenv(info.environmentVariable);
        if (xdgValue != nullptr && std::strlen(xdgValue) != 0)
        {
            baseDirectory = std::filesystem::path(xdgValue);
        }
        else
        {
            std::filesystem::path homePath;
            Status status = retinify::HomeDirectoryPath(homePath);
            if (!status.IsOK())
            {
                path.clear();
                return status;
            }

            baseDirectory = homePath / info.fallbackRelativeToHome;
        }

        const char *version = retinify::Version();
        if (version == nullptr || std::strlen(version) == 0)
        {
            retinify::LogError("Version string is null or empty.");
            path.clear();
            return Status(StatusCategory::RETINIFY, StatusCode::FAIL);
        }

        const std::filesystem::path fullPath = baseDirectory / kRetinifyDirName / version;

        std::error_code ec;
        std::filesystem::create_directories(fullPath, ec);
        if (ec)
        {
            retinify::LogError(ec.message().c_str());
            path.clear();
            return Status(StatusCategory::SYSTEM, StatusCode::FAIL);
        }

        path = fullPath;
        return Status{};
    }
    catch (const std::exception &ex)
    {
        retinify::LogError(ex.what());
    }
    catch (...)
    {
        retinify::LogError("Unknown exception occurred.");
    }

    path.clear();
    return Status(StatusCategory::SYSTEM, StatusCode::FAIL);
}
} // namespace

auto HomeDirectoryPath(std::filesystem::path &path) noexcept -> Status
{
    try
    {
        const char *homePath = std::getenv("HOME");
        if (homePath == nullptr || std::strlen(homePath) == 0)
        {
            retinify::LogError("HOME environment variable is not set or empty.");
            path.clear();
            return Status(StatusCategory::SYSTEM, StatusCode::FAIL);
        }

        path = std::filesystem::path{homePath};
        return Status{};
    }
    catch (const std::exception &ex)
    {
        retinify::LogError(ex.what());
    }
    catch (...)
    {
        retinify::LogError("Unknown exception occurred.");
    }

    path.clear();
    return Status(StatusCategory::SYSTEM, StatusCode::FAIL);
}

auto ConfigDirectoryPath(std::filesystem::path &path) noexcept -> Status
{
    return GetUserDirectoryPath(kConfigDirectoryInfo, path);
}

auto CacheDirectoryPath(std::filesystem::path &path) noexcept -> Status
{
    return GetUserDirectoryPath(kCacheDirectoryInfo, path);
}

auto DataDirectoryPath(std::filesystem::path &path) noexcept -> Status
{
    return GetUserDirectoryPath(kDataDirectoryInfo, path);
}

auto StateDirectoryPath(std::filesystem::path &path) noexcept -> Status
{
    return GetUserDirectoryPath(kStateDirectoryInfo, path);
}
} // namespace retinify
