// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Resources.hpp>
#include <glad/glad.h>

namespace VRHI {

/// Utility class for OpenGL format conversions
/// Designed to be extensible for other OpenGL versions (3.3, 4.1, 4.6, etc.)
class GLFormatUtils {
public:
    /// Convert VRHI TextureFormat to OpenGL format and type parameters
    /// @param format VRHI texture format
    /// @param outFormat Output: OpenGL format (e.g., GL_RGBA, GL_RED)
    /// @param outType Output: OpenGL type (e.g., GL_UNSIGNED_BYTE, GL_FLOAT)
    static void GetFormatAndType(TextureFormat format, GLenum& outFormat, GLenum& outType);
    
    /// Convert VRHI TextureFormat to OpenGL internal format
    /// @param format VRHI texture format
    /// @return OpenGL internal format (e.g., GL_RGBA8, GL_R32F)
    static GLenum GetInternalFormat(TextureFormat format);
    
    /// Convert VRHI TextureType to OpenGL texture target
    /// @param type VRHI texture type
    /// @return OpenGL texture target (e.g., GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP)
    static GLenum GetTextureTarget(TextureType type);
    
    /// Check if a texture format is a compressed format
    /// @param format VRHI texture format
    /// @return True if compressed, false otherwise
    static bool IsCompressedFormat(TextureFormat format);
    
    /// Check if a texture format is a depth/stencil format
    /// @param format VRHI texture format
    /// @return True if depth/stencil, false otherwise
    static bool IsDepthStencilFormat(TextureFormat format);
};

} // namespace VRHI
