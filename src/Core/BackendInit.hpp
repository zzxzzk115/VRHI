// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

namespace VRHI {

/// Initialize all available backends
/// This function ensures all backend static initializers are executed
void InitializeBackends();

} // namespace VRHI
