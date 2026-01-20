// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "retinify/paths.hpp"
#include "retinify/status.hpp"

#include <filesystem>
#include <gtest/gtest.h>

namespace retinify
{
class PathTest : public ::testing::Test
{
  protected:
    using PathResolver = Status (*)(std::filesystem::path &);

    void CheckPath(const std::filesystem::path &path, bool shouldBeDirectory)
    {
        ASSERT_FALSE(path.empty()) << "Path is empty.";

        std::error_code errorCode;
        const bool exists = std::filesystem::exists(path, errorCode);
        ASSERT_FALSE(errorCode) << "Error checking path existence: " << errorCode.message() << " for path: " << path;
        ASSERT_TRUE(exists) << "Path does not exist: " << path;

        const auto status = std::filesystem::status(path, errorCode);
        ASSERT_FALSE(errorCode) << "Error getting file status: " << errorCode.message() << " for path: " << path;

        if (shouldBeDirectory)
        {
            ASSERT_TRUE(std::filesystem::is_directory(status)) << "Path is not a directory: " << path << " (actual type: " << static_cast<int>(status.type()) << ")";
        }
        else
        {
            ASSERT_TRUE(std::filesystem::is_regular_file(status)) << "Path is not a regular file: " << path << " (actual type: " << static_cast<int>(status.type()) << ")";
        }
    }

    void ExpectPath(PathResolver resolver, bool shouldBeDirectory)
    {
        std::filesystem::path path;
        const Status status = resolver(path);
        ASSERT_TRUE(status.IsOK()) << "Failed to resolve path.";
        CheckPath(path, shouldBeDirectory);
    }
};

TEST_F(PathTest, HomeDirectoryPath)
{
    ExpectPath(HomeDirectoryPath, true);
}

TEST_F(PathTest, ConfigDirectoryPath)
{
    ExpectPath(ConfigDirectoryPath, true);
}

TEST_F(PathTest, CacheDirectoryPath)
{
    ExpectPath(CacheDirectoryPath, true);
}

TEST_F(PathTest, DataDirectoryPath)
{
    ExpectPath(DataDirectoryPath, true);
}

TEST_F(PathTest, StateDirectoryPath)
{
    ExpectPath(StateDirectoryPath, true);
}
} // namespace retinify
