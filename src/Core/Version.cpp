// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

/**
 * @file Version.cpp
 * @brief VRHI version information
 * 
 * This is a placeholder implementation file to demonstrate the build system.
 * Actual implementation will be added during the development phase.
 */

#include <VRHI/VRHI.hpp>

namespace VRHI {

Version GetVersion() noexcept {
    return Version{
        .major = 1,
        .minor = 0,
        .patch = 0,
        .string = "1.0.0"
    };
}

} // namespace VRHI
