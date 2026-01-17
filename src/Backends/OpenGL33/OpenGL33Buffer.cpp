// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Buffer.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

namespace {
    GLenum GetGLBufferTarget(BufferUsage usage) {
        if (static_cast<uint32_t>(usage & BufferUsage::Index)) {
            return GL_ELEMENT_ARRAY_BUFFER;
        }
        if (static_cast<uint32_t>(usage & BufferUsage::Uniform)) {
            return GL_UNIFORM_BUFFER;
        }
        // Default to array buffer for vertex and other types
        return GL_ARRAY_BUFFER;
    }
    
    GLenum GetGLBufferUsage(MemoryAccess access) {
        switch (access) {
            case MemoryAccess::GpuOnly:
                return GL_STATIC_DRAW;
            case MemoryAccess::CpuToGpu:
                return GL_DYNAMIC_DRAW;
            case MemoryAccess::GpuToCpu:
                return GL_STREAM_READ;
            case MemoryAccess::CpuOnly:
                return GL_STREAM_DRAW;
            default:
                return GL_STATIC_DRAW;
        }
    }
}

OpenGL33Buffer::OpenGL33Buffer(const BufferDesc& desc, GLuint buffer, GLenum target)
    : m_desc(desc)
    , m_buffer(buffer)
    , m_target(target)
{
}

OpenGL33Buffer::~OpenGL33Buffer() {
    if (m_buffer != 0) {
        glDeleteBuffers(1, &m_buffer);
    }
}

std::expected<std::unique_ptr<Buffer>, Error>
OpenGL33Buffer::Create(const BufferDesc& desc) {
    if (desc.size == 0) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Buffer size must be greater than 0"
        });
    }
    
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    
    if (buffer == 0) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to create OpenGL buffer"
        });
    }
    
    GLenum target = GetGLBufferTarget(desc.usage);
    GLenum usage = GetGLBufferUsage(desc.memoryAccess);
    
    glBindBuffer(target, buffer);
    glBufferData(target, desc.size, desc.initialData, usage);
    
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        glDeleteBuffers(1, &buffer);
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to allocate buffer data"
        });
    }
    
    glBindBuffer(target, 0);
    
    auto bufferObj = std::unique_ptr<Buffer>(
        new OpenGL33Buffer(desc, buffer, target)
    );
    
    return bufferObj;
}

size_t OpenGL33Buffer::GetSize() const noexcept {
    return m_desc.size;
}

BufferUsage OpenGL33Buffer::GetUsage() const noexcept {
    return m_desc.usage;
}

void* OpenGL33Buffer::Map() {
    return Map(0, m_desc.size);
}

void* OpenGL33Buffer::Map(size_t offset, size_t size) {
    if (m_mappedPtr != nullptr) {
        LogWarning("Buffer already mapped");
        return m_mappedPtr;
    }
    
    glBindBuffer(m_target, m_buffer);
    
    // GL 3.0+ supports glMapBufferRange
    GLbitfield access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
    m_mappedPtr = glMapBufferRange(m_target, offset, size, access);
    
    if (m_mappedPtr == nullptr) {
        LogError("Failed to map buffer");
    }
    
    glBindBuffer(m_target, 0);
    
    return m_mappedPtr;
}

void OpenGL33Buffer::Unmap() {
    if (m_mappedPtr == nullptr) {
        return;
    }
    
    glBindBuffer(m_target, m_buffer);
    glUnmapBuffer(m_target);
    glBindBuffer(m_target, 0);
    
    m_mappedPtr = nullptr;
}

void OpenGL33Buffer::Update(const void* data, size_t size, size_t offset) {
    if (offset + size > m_desc.size) {
        LogError("Buffer update out of bounds");
        return;
    }
    
    glBindBuffer(m_target, m_buffer);
    glBufferSubData(m_target, offset, size, data);
    glBindBuffer(m_target, 0);
}

void OpenGL33Buffer::Read(void* data, size_t size, size_t offset) {
    if (offset + size > m_desc.size) {
        LogError("Buffer read out of bounds");
        return;
    }
    
    glBindBuffer(m_target, m_buffer);
    glGetBufferSubData(m_target, offset, size, data);
    glBindBuffer(m_target, 0);
}

} // namespace VRHI
