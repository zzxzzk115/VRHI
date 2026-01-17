// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHI.hpp>

namespace VRHI {

class OpenGL33SwapChain : public SwapChain {
public:
    OpenGL33SwapChain() = default;
    ~OpenGL33SwapChain() override = default;
};

} // namespace VRHI
