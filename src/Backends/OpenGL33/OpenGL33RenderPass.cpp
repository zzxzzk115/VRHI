// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33RenderPass.hpp"

namespace VRHI {

std::expected<std::unique_ptr<RenderPass>, Error>
OpenGL33RenderPass::Create(const RenderPassDesc& desc) {
    // In OpenGL, render passes are not explicit objects
    // We just store the descriptor for later use
    return std::unique_ptr<RenderPass>(new OpenGL33RenderPass());
}

} // namespace VRHI
