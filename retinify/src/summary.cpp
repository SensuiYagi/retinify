// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "summary.hpp"

#include "retinify/logging.hpp"
#include "retinify/version.hpp"

#include <atomic>
#include <cstdio>

namespace retinify
{
void LogSoftwareSummary() noexcept
{
    static std::atomic_flag printed = ATOMIC_FLAG_INIT;
    if (printed.test_and_set())
    {
        return;
    }

    char summaryBuffer[128];
    std::snprintf(summaryBuffer, sizeof(summaryBuffer), "retinify v%s | Real-Time AI Stereo Vision Library | Copyright (c) 2025 Sensui Yagi", Version());
    LogInfo(summaryBuffer);
}
} // namespace retinify
