// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once
#include <VRHI/VRHI.hpp>
#include <expected>
#include <memory>

#include <VRHI/RenderPass.hpp>

namespace VRHI {

class OpenGL33RenderPass : public RenderPass {
public:
    ~OpenGL33RenderPass() override = default;
    
    static std::expected<std::unique_ptr<RenderPass>, Error>
    Create(const RenderPassDesc& desc);
    
private:
    OpenGL33RenderPass() = default;
};

} // namespace VRHI
