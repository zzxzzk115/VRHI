// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHI.hpp>
#include <VRHI/Resources.hpp>
#include <glad/glad.h>
#include <expected>
#include <memory>

namespace VRHI {

/// OpenGL 3.3 buffer implementation
class OpenGL33Buffer : public Buffer {
public:
    ~OpenGL33Buffer() override;
    
    static std::expected<std::unique_ptr<Buffer>, Error>
    Create(const BufferDesc& desc);
    
    // Buffer interface
    size_t GetSize() const noexcept override;
    BufferUsage GetUsage() const noexcept override;
    
    void* Map() override;
    void* Map(size_t offset, size_t size) override;
    void Unmap() override;
    
    void Update(const void* data, size_t size, size_t offset = 0) override;
    void Read(void* data, size_t size, size_t offset = 0) override;
    
    // OpenGL-specific
    GLuint GetHandle() const noexcept { return m_buffer; }
    GLenum GetTarget() const noexcept { return m_target; }
    
private:
    OpenGL33Buffer(const BufferDesc& desc, GLuint buffer, GLenum target);
    
    BufferDesc m_desc;
    GLuint m_buffer = 0;
    GLenum m_target = GL_ARRAY_BUFFER;
    void* m_mappedPtr = nullptr;
};

} // namespace VRHI
