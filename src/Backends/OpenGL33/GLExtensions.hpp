// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

namespace VRHI {

/// Common OpenGL extension names
/// These string constants are used for runtime extension checking
/// The actual GL constants are now provided by GLAD
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
