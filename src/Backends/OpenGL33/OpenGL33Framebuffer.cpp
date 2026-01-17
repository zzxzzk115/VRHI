// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Framebuffer.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

OpenGL33Framebuffer::~OpenGL33Framebuffer() {
    if (m_framebuffer != 0) {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

std::expected<std::unique_ptr<Framebuffer>, Error>
OpenGL33Framebuffer::Create(const FramebufferDesc& desc) {
    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    
    if (framebuffer == 0) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to create OpenGL framebuffer"
        });
    }
    
    // Simplified implementation - in a real implementation, we would:
    // 1. Bind the framebuffer
    // 2. Attach color/depth textures from desc
    // 3. Check framebuffer completeness
    // 4. Unbind
    
    // For now, assume default dimensions
    uint32_t width = 1280;
    uint32_t height = 720;
    uint32_t layers = 1;
    
    return std::unique_ptr<Framebuffer>(new OpenGL33Framebuffer(framebuffer, width, height, layers));
}

} // namespace VRHI
