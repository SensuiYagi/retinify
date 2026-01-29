// SPDX-FileCopyrightText: Copyright (c) 2025 Sensui Yagi. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <exception>
#include <type_traits>
#include <utility>

#include "retinify/logging.hpp"
#include "retinify/status.hpp"

namespace retinify
{
/// @brief
/// Executes a function and converts any exception into a Status.
/// @tparam Function
/// The type of the function to execute.
/// @param function
/// The function to execute.
/// @return
/// A Status object that indicates whether the operation was successful
template <typename Function> auto NoThrow(Function &&function) noexcept -> Status
{
    static_assert(std::is_same_v<std::invoke_result_t<Function>, Status>, "NoThrow requires Function to return Status");

    try
    {
        return std::forward<Function>(function)();
    }
    catch (const std::exception &ex)
    {
        LogError(ex.what());
        return Status{StatusCategory::RETINIFY, StatusCode::FAIL};
    }
    catch (...)
    {
        LogError("Unknown exception caught");
        return Status{StatusCategory::RETINIFY, StatusCode::FAIL};
    }
}
} // namespace retinify