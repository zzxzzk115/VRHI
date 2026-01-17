// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once
#include <VRHI/VRHI.hpp>
#include <expected>
#include <memory>

#include <VRHI/RenderPass.hpp>
#include <glad/glad.h>

namespace VRHI {

class OpenGL33Framebuffer : public Framebuffer {
public:
    ~OpenGL33Framebuffer() override;
    
    static std::expected<std::unique_ptr<Framebuffer>, Error>
    Create(const FramebufferDesc& desc);
    
    uint32_t GetWidth() const noexcept override { return m_width; }
    uint32_t GetHeight() const noexcept override { return m_height; }
    uint32_t GetLayers() const noexcept override { return m_layers; }
    
    GLuint GetHandle() const noexcept { return m_framebuffer; }
    
private:
    OpenGL33Framebuffer(GLuint framebuffer, uint32_t width, uint32_t height, uint32_t layers)
        : m_framebuffer(framebuffer), m_width(width), m_height(height), m_layers(layers) {}
    
    GLuint m_framebuffer = 0;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_layers = 1;
};

} // namespace VRHI
