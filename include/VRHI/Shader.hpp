// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <string_view>
#include <span>

namespace VRHI {

// ============================================================================
// Shader Stage
// ============================================================================

enum class ShaderStage : uint32_t {
    Vertex        = 1 << 0,
    Fragment      = 1 << 1,
    Geometry      = 1 << 2,
    TessControl   = 1 << 3,
    TessEval      = 1 << 4,
    Compute       = 1 << 5,
    Mesh          = 1 << 6,
    Task          = 1 << 7,
};

// Bitwise operators for ShaderStage
constexpr ShaderStage operator|(ShaderStage lhs, ShaderStage rhs) noexcept {
    return static_cast<ShaderStage>(
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
    );
}

constexpr ShaderStage operator&(ShaderStage lhs, ShaderStage rhs) noexcept {
    return static_cast<ShaderStage>(
        static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
    );
}

constexpr ShaderStage& operator|=(ShaderStage& lhs, ShaderStage rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

constexpr ShaderStage& operator&=(ShaderStage& lhs, ShaderStage rhs) noexcept {
    lhs = lhs & rhs;
    return lhs;
}

// ============================================================================
// Shader Language
// ============================================================================

enum class ShaderLanguage {
    GLSL,        // GLSL source code
    HLSL,        // HLSL source code
    SPIRV,       // SPIR-V bytecode
    MSL,         // Metal Shading Language
};

// ============================================================================
// Shader Descriptor
// ============================================================================

struct ShaderDesc {
    ShaderStage stage = ShaderStage::Vertex;
    ShaderLanguage language = ShaderLanguage::GLSL;
    
    // Source code or bytecode
    const void* code = nullptr;
    size_t codeSize = 0;
    
    // Entry point function name (default: "main")
    const char* entryPoint = "main";
    
    // Optional debug name
    const char* debugName = nullptr;
};

// ============================================================================
// Shader Interface
// ============================================================================

/// Shader module interface
class IShader {
public:
    virtual ~IShader() = default;
    
    // Shader cannot be copied, only moved
    IShader(const IShader&) = delete;
    IShader& operator=(const IShader&) = delete;
    IShader(IShader&&) noexcept = default;
    IShader& operator=(IShader&&) noexcept = default;
    
    // ========================================================================
    // Shader Information
    // ========================================================================
    
    /// Get shader stage
    virtual ShaderStage GetStage() const noexcept = 0;
    
    /// Get shader language
    virtual ShaderLanguage GetLanguage() const noexcept = 0;
    
    /// Get entry point name
    virtual std::string_view GetEntryPoint() const noexcept = 0;
    
    // ========================================================================
    // Native Handle Access (Optional)
    // ========================================================================
    
    /// Get native shader handle for interoperability
    /// @return Native handle or nullptr if not supported
    virtual void* GetNativeHandle() const noexcept { return nullptr; }
    
protected:
    IShader() = default;
};

// Convenience alias for the public API
using Shader = IShader;

} // namespace VRHI
