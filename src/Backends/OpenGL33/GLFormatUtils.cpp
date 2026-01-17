// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "GLFormatUtils.hpp"

namespace VRHI {

void GLFormatUtils::GetFormatAndType(TextureFormat format, GLenum& outFormat, GLenum& outType) {
    switch (format) {
        // 8-bit formats
        case TextureFormat::R8_UNorm:
            outFormat = GL_RED;
            outType = GL_UNSIGNED_BYTE;
            break;
        case TextureFormat::RG8_UNorm:
            outFormat = GL_RG;
            outType = GL_UNSIGNED_BYTE;
            break;
        case TextureFormat::RGBA8_UNorm:
        case TextureFormat::RGBA8_SRGB:
            outFormat = GL_RGBA;
            outType = GL_UNSIGNED_BYTE;
            break;
            
        // 16-bit float formats
        case TextureFormat::R16_Float:
            outFormat = GL_RED;
            outType = GL_HALF_FLOAT;
            break;
        case TextureFormat::RG16_Float:
            outFormat = GL_RG;
            outType = GL_HALF_FLOAT;
            break;
        case TextureFormat::RGBA16_Float:
            outFormat = GL_RGBA;
            outType = GL_HALF_FLOAT;
            break;
            
        // 32-bit float formats
        case TextureFormat::R32_Float:
            outFormat = GL_RED;
            outType = GL_FLOAT;
            break;
        case TextureFormat::RG32_Float:
            outFormat = GL_RG;
            outType = GL_FLOAT;
            break;
        case TextureFormat::RGB32_Float:
            outFormat = GL_RGB;
            outType = GL_FLOAT;
            break;
        case TextureFormat::RGBA32_Float:
            outFormat = GL_RGBA;
            outType = GL_FLOAT;
            break;
            
        // 32-bit unsigned integer formats
        case TextureFormat::R32_UInt:
            outFormat = GL_RED_INTEGER;
            outType = GL_UNSIGNED_INT;
            break;
        case TextureFormat::RG32_UInt:
            outFormat = GL_RG_INTEGER;
            outType = GL_UNSIGNED_INT;
            break;
        case TextureFormat::RGB32_UInt:
            outFormat = GL_RGB_INTEGER;
            outType = GL_UNSIGNED_INT;
            break;
        case TextureFormat::RGBA32_UInt:
            outFormat = GL_RGBA_INTEGER;
            outType = GL_UNSIGNED_INT;
            break;
            
        // Depth/stencil formats
        case TextureFormat::Depth16:
            outFormat = GL_DEPTH_COMPONENT;
            outType = GL_UNSIGNED_SHORT;
            break;
        case TextureFormat::Depth24Stencil8:
            outFormat = GL_DEPTH_STENCIL;
            outType = GL_UNSIGNED_INT_24_8;
            break;
        case TextureFormat::Depth32F:
            outFormat = GL_DEPTH_COMPONENT;
            outType = GL_FLOAT;
            break;
        case TextureFormat::Depth32FStencil8:
            outFormat = GL_DEPTH_STENCIL;
            outType = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
            break;
            
        // Compressed formats - these don't have format/type, only internal format
        case TextureFormat::BC1_UNorm:
        case TextureFormat::BC3_UNorm:
        case TextureFormat::BC7_UNorm:
        case TextureFormat::ETC2_RGB8:
        case TextureFormat::ASTC_4x4:
            outFormat = GL_RGBA;  // Placeholder, not used for compressed
            outType = GL_UNSIGNED_BYTE;
            break;
            
        default:
            outFormat = GL_RGBA;
            outType = GL_UNSIGNED_BYTE;
            break;
    }
}

GLenum GLFormatUtils::GetInternalFormat(TextureFormat format) {
    switch (format) {
        // 8-bit formats
        case TextureFormat::R8_UNorm: return GL_R8;
        case TextureFormat::RG8_UNorm: return GL_RG8;
        case TextureFormat::RGBA8_UNorm: return GL_RGBA8;
        case TextureFormat::RGBA8_SRGB: return GL_SRGB8_ALPHA8;
        
        // 16-bit float formats
        case TextureFormat::R16_Float: return GL_R16F;
        case TextureFormat::RG16_Float: return GL_RG16F;
        case TextureFormat::RGBA16_Float: return GL_RGBA16F;
        
        // 32-bit float formats
        case TextureFormat::R32_Float: return GL_R32F;
        case TextureFormat::RG32_Float: return GL_RG32F;
        case TextureFormat::RGB32_Float: return GL_RGB32F;
        case TextureFormat::RGBA32_Float: return GL_RGBA32F;
        
        // 32-bit unsigned integer formats
        case TextureFormat::R32_UInt: return GL_R32UI;
        case TextureFormat::RG32_UInt: return GL_RG32UI;
        case TextureFormat::RGB32_UInt: return GL_RGB32UI;
        case TextureFormat::RGBA32_UInt: return GL_RGBA32UI;
        
        // Depth/stencil formats
        case TextureFormat::Depth16: return GL_DEPTH_COMPONENT16;
        case TextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
        case TextureFormat::Depth32F: return GL_DEPTH_COMPONENT32F;
        case TextureFormat::Depth32FStencil8: return GL_DEPTH32F_STENCIL8;
        
        // Compressed formats - using constants that work across GL versions
        case TextureFormat::BC1_UNorm: return 0x83F1;  // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
        case TextureFormat::BC3_UNorm: return 0x83F3;  // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
        case TextureFormat::BC7_UNorm: return 0x8E8C;  // GL_COMPRESSED_RGBA_BPTC_UNORM
        case TextureFormat::ETC2_RGB8: return 0x9274;  // GL_COMPRESSED_RGB8_ETC2
        case TextureFormat::ASTC_4x4: return 0x93B0;   // GL_COMPRESSED_RGBA_ASTC_4x4_KHR
        
        default: return GL_RGBA8;
    }
}

GLenum GLFormatUtils::GetTextureTarget(TextureType type) {
    switch (type) {
        case TextureType::Texture1D: return GL_TEXTURE_1D;
        case TextureType::Texture2D: return GL_TEXTURE_2D;
        case TextureType::Texture3D: return GL_TEXTURE_3D;
        case TextureType::TextureCube: return GL_TEXTURE_CUBE_MAP;
        case TextureType::Texture1DArray: return GL_TEXTURE_1D_ARRAY;
        case TextureType::Texture2DArray: return GL_TEXTURE_2D_ARRAY;
        case TextureType::TextureCubeArray:
            // GL_TEXTURE_CUBE_MAP_ARRAY is GL 4.0+, not available in 3.3
            // Fall back to regular cube map
            return GL_TEXTURE_CUBE_MAP;
        default: return GL_TEXTURE_2D;
    }
}

bool GLFormatUtils::IsCompressedFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::BC1_UNorm:
        case TextureFormat::BC3_UNorm:
        case TextureFormat::BC7_UNorm:
        case TextureFormat::ETC2_RGB8:
        case TextureFormat::ASTC_4x4:
            return true;
        default:
            return false;
    }
}

bool GLFormatUtils::IsDepthStencilFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::Depth16:
        case TextureFormat::Depth24Stencil8:
        case TextureFormat::Depth32F:
        case TextureFormat::Depth32FStencil8:
            return true;
        default:
            return false;
    }
}

} // namespace VRHI
