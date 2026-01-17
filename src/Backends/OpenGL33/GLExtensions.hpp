// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <glad/glad.h>

namespace VRHI {

/// OpenGL extension constants
/// These are defined here for clarity and to avoid magic numbers in the code
/// Extensions are commonly supported across different OpenGL versions
namespace GLExtConstants {
    // Texture compression extensions (S3TC/DXT)
    // Extension: GL_EXT_texture_compression_s3tc
    constexpr GLenum GL_COMPRESSED_RGB_S3TC_DXT1_EXT = 0x83F0;
    constexpr GLenum GL_COMPRESSED_RGBA_S3TC_DXT1_EXT = 0x83F1;
    constexpr GLenum GL_COMPRESSED_RGBA_S3TC_DXT3_EXT = 0x83F2;
    constexpr GLenum GL_COMPRESSED_RGBA_S3TC_DXT5_EXT = 0x83F3;
    
    // Texture compression extensions (BPTC)
    // Extension: GL_ARB_texture_compression_bptc (GL 4.2+)
    constexpr GLenum GL_COMPRESSED_RGBA_BPTC_UNORM = 0x8E8C;
    constexpr GLenum GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM = 0x8E8D;
    constexpr GLenum GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT = 0x8E8E;
    constexpr GLenum GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = 0x8E8F;
    
    // Texture compression extensions (ETC2)
    // Extension: GL_ARB_ES3_compatibility (GL 4.3+) or core in GLES 3.0+
    constexpr GLenum GL_COMPRESSED_RGB8_ETC2 = 0x9274;
    constexpr GLenum GL_COMPRESSED_SRGB8_ETC2 = 0x9275;
    constexpr GLenum GL_COMPRESSED_RGBA8_ETC2_EAC = 0x9278;
    constexpr GLenum GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC = 0x9279;
    
    // Texture compression extensions (ASTC)
    // Extension: GL_KHR_texture_compression_astc_ldr
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_4x4_KHR = 0x93B0;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_5x4_KHR = 0x93B1;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_5x5_KHR = 0x93B2;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_6x5_KHR = 0x93B3;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_6x6_KHR = 0x93B4;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_8x5_KHR = 0x93B5;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_8x6_KHR = 0x93B6;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_8x8_KHR = 0x93B7;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_10x5_KHR = 0x93B8;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_10x6_KHR = 0x93B9;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_10x8_KHR = 0x93BA;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_10x10_KHR = 0x93BB;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_12x10_KHR = 0x93BC;
    constexpr GLenum GL_COMPRESSED_RGBA_ASTC_12x12_KHR = 0x93BD;
    
    // Anisotropic filtering extension
    // Extension: GL_EXT_texture_filter_anisotropic
    constexpr GLenum GL_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FE;
    constexpr GLenum GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FF;
}

/// Common OpenGL extensions that should be checked
/// These are widely supported and useful for rendering
struct GLCommonExtensions {
    // Texture compression
    static constexpr const char* EXT_texture_compression_s3tc = "GL_EXT_texture_compression_s3tc";
    static constexpr const char* ARB_texture_compression_bptc = "GL_ARB_texture_compression_bptc";
    static constexpr const char* ARB_ES3_compatibility = "GL_ARB_ES3_compatibility";
    static constexpr const char* KHR_texture_compression_astc_ldr = "GL_KHR_texture_compression_astc_ldr";
    
    // Filtering and sampling
    static constexpr const char* EXT_texture_filter_anisotropic = "GL_EXT_texture_filter_anisotropic";
    static constexpr const char* ARB_seamless_cube_map = "GL_ARB_seamless_cube_map";
    
    // Debug and validation
    static constexpr const char* KHR_debug = "GL_KHR_debug";
    static constexpr const char* ARB_debug_output = "GL_ARB_debug_output";
    
    // Advanced features
    static constexpr const char* ARB_bindless_texture = "GL_ARB_bindless_texture";
    static constexpr const char* ARB_multi_draw_indirect = "GL_ARB_multi_draw_indirect";
    static constexpr const char* ARB_shader_storage_buffer_object = "GL_ARB_shader_storage_buffer_object";
    static constexpr const char* ARB_compute_shader = "GL_ARB_compute_shader";
    static constexpr const char* ARB_tessellation_shader = "GL_ARB_tessellation_shader";
    static constexpr const char* ARB_geometry_shader4 = "GL_ARB_geometry_shader4";
};

} // namespace VRHI
