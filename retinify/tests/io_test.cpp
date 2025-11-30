// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "retinify/io.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <random>
#include <stdexcept>
#include <string>

namespace retinify
{
namespace
{
class ScopedTempDir
{
  public:
    ScopedTempDir()
    {
        const auto base = std::filesystem::temp_directory_path();
        std::random_device rd;

        for (int attempt = 0; attempt < 32; ++attempt)
        {
            const auto candidate = base / ("retinify-io-test-" + std::to_string(rd()) + "-" + std::to_string(attempt));

            std::error_code ec;
            if (std::filesystem::create_directory(candidate, ec))
            {
                path_ = candidate;
                return;
            }

            if (!ec && std::filesystem::is_directory(candidate))
            {
                path_ = candidate;
                return;
            }
        }

        throw std::runtime_error("Unable to create temporary directory for io tests");
    }

    ~ScopedTempDir()
    {
        std::error_code ec;
        std::filesystem::remove_all(path_, ec);
    }

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &
    {
        return path_;
    }

  private:
    std::filesystem::path path_;
};

[[nodiscard]] auto MakeSampleParameters() -> CalibrationParameters
{
    CalibrationParameters params{};

    params.leftIntrinsics.fx = 610.25;
    params.leftIntrinsics.fy = 612.5;
    params.leftIntrinsics.cx = 320.75;
    params.leftIntrinsics.cy = 240.125;
    params.leftIntrinsics.skew = -0.0125;

    params.leftDistortion.k1 = -0.01;
    params.leftDistortion.k2 = 0.002;
    params.leftDistortion.p1 = -0.0003;
    params.leftDistortion.p2 = 0.0004;
    params.leftDistortion.k3 = -0.0005;
    params.leftDistortion.k4 = 0.0006;
    params.leftDistortion.k5 = -0.0007;
    params.leftDistortion.k6 = 0.0008;

    params.rightIntrinsics.fx = 605.75;
    params.rightIntrinsics.fy = 607.0;
    params.rightIntrinsics.cx = 318.25;
    params.rightIntrinsics.cy = 242.5;
    params.rightIntrinsics.skew = 0.025;

    params.rightDistortion.k1 = 0.011;
    params.rightDistortion.k2 = -0.0022;
    params.rightDistortion.p1 = 0.00033;
    params.rightDistortion.p2 = -0.00044;
    params.rightDistortion.k3 = 0.00055;
    params.rightDistortion.k4 = -0.00066;
    params.rightDistortion.k5 = 0.00077;
    params.rightDistortion.k6 = -0.00088;

    params.rotation = {{{0.9998, -0.0175, 0.0042}, {0.0176, 0.9997, -0.0123}, {-0.0040, 0.0124, 0.9999}}};

    params.translation = {0.105, -0.208, 0.315};

    params.imageWidth = 640;
    params.imageHeight = 480;
    params.calibrationError = 0.25;
    params.calibrationTime = 1720000000123456789ll;

    return params;
}

class IoTest : public ::testing::Test
{
  protected:
    [[nodiscard]] auto Path(const std::string &filename) const -> std::filesystem::path
    {
        return tempDir_.path() / filename;
    }

    [[nodiscard]] auto Save(const std::filesystem::path &path, const CalibrationParameters &params) const -> Status
    {
        const auto pathString = path.string();
        return SaveCalibrationParameters(pathString.c_str(), params);
    }

    [[nodiscard]] auto Load(const std::filesystem::path &path, CalibrationParameters &params) const -> Status
    {
        const auto pathString = path.string();
        return LoadCalibrationParameters(pathString.c_str(), params);
    }

    ScopedTempDir tempDir_;
    CalibrationParameters sample_{MakeSampleParameters()};
};
} // namespace

TEST_F(IoTest, SaveAndLoadRoundTrip)
{
    const auto filePath = Path("calibration.json");

    const auto saveStatus = Save(filePath, sample_);
    ASSERT_TRUE(saveStatus.IsOK());

    CalibrationParameters loaded{};
    const auto loadStatus = Load(filePath, loaded);
    ASSERT_TRUE(loadStatus.IsOK());

    EXPECT_EQ(sample_, loaded);
}

TEST_F(IoTest, SaveRejectsInvalidFilename)
{
    const auto nullStatus = SaveCalibrationParameters(nullptr, sample_);
    EXPECT_FALSE(nullStatus.IsOK());
    EXPECT_EQ(nullStatus.Category(), StatusCategory::USER);
    EXPECT_EQ(nullStatus.Code(), StatusCode::INVALID_ARGUMENT);

    const auto emptyStatus = SaveCalibrationParameters("", sample_);
    EXPECT_FALSE(emptyStatus.IsOK());
    EXPECT_EQ(emptyStatus.Category(), StatusCategory::USER);
    EXPECT_EQ(emptyStatus.Code(), StatusCode::INVALID_ARGUMENT);
}

TEST_F(IoTest, LoadRejectsInvalidFilename)
{
    CalibrationParameters params{};

    const auto nullStatus = LoadCalibrationParameters(nullptr, params);
    EXPECT_FALSE(nullStatus.IsOK());
    EXPECT_EQ(nullStatus.Category(), StatusCategory::USER);
    EXPECT_EQ(nullStatus.Code(), StatusCode::INVALID_ARGUMENT);

    const auto emptyStatus = LoadCalibrationParameters("", params);
    EXPECT_FALSE(emptyStatus.IsOK());
    EXPECT_EQ(emptyStatus.Category(), StatusCategory::USER);
    EXPECT_EQ(emptyStatus.Code(), StatusCode::INVALID_ARGUMENT);
}

TEST_F(IoTest, SaveFailsWhenParentIsFile)
{
    const auto parent = Path("not-a-directory");
    {
        std::ofstream marker(parent);
        ASSERT_TRUE(marker.is_open());
    }

    const auto target = parent / "calibration.json";

    const auto status = Save(target, sample_);
    EXPECT_FALSE(status.IsOK());
    EXPECT_EQ(status.Category(), StatusCategory::SYSTEM);
    EXPECT_EQ(status.Code(), StatusCode::FAIL);

    std::error_code ec;
    EXPECT_FALSE(std::filesystem::exists(target, ec));
}

TEST_F(IoTest, LoadFailsForMissingFile)
{
    const auto missing = Path("does-not-exist.json");

    CalibrationParameters params{};
    const auto status = Load(missing, params);

    EXPECT_FALSE(status.IsOK());
    EXPECT_EQ(status.Category(), StatusCategory::SYSTEM);
    EXPECT_EQ(status.Code(), StatusCode::FAIL);
}

TEST_F(IoTest, SavesZeroRotationAndTranslationAsZeros)
{
    CalibrationParameters params;
    const auto filePath = Path("zero-rt.json");

    ASSERT_TRUE(Save(filePath, params).IsOK());

    CalibrationParameters loaded{};
    ASSERT_TRUE(Load(filePath, loaded).IsOK());

    EXPECT_EQ(params, loaded);
}
} // namespace retinify
