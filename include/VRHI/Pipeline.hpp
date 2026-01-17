// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include "Shader.hpp"
#include "Resources.hpp"
#include <cstdint>
#include <span>

namespace VRHI {

// Forward declarations
class Shader;
class RenderPass;

// ============================================================================
// Pipeline Type
// ============================================================================

enum class PipelineType {
    Graphics,
    Compute,
};

// ============================================================================
// Vertex Input
// ============================================================================

enum class VertexInputRate {
    Vertex,      // Per-vertex data
    Instance,    // Per-instance data
};

enum class VertexFormat {
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    UInt,
    UInt2,
    UInt3,
    UInt4,
};

struct VertexAttribute {
    uint32_t location = 0;
    uint32_t binding = 0;
    VertexFormat format = VertexFormat::Float3;
    uint32_t offset = 0;
};

struct VertexBinding {
    uint32_t binding = 0;
    uint32_t stride = 0;
    VertexInputRate inputRate = VertexInputRate::Vertex;
};

struct VertexInputState {
    std::span<const VertexAttribute> attributes;
    std::span<const VertexBinding> bindings;
};

// ============================================================================
// Input Assembly
// ============================================================================

enum class PrimitiveTopology {
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    TriangleFan,
    LineListWithAdjacency,
    LineStripWithAdjacency,
    TriangleListWithAdjacency,
    TriangleStripWithAdjacency,
    PatchList,
};

struct InputAssemblyState {
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    bool primitiveRestartEnable = false;
};

// ============================================================================
// Rasterization
// ============================================================================

enum class PolygonMode {
    Fill,
    Line,
    Point,
};

enum class CullMode {
    None,
    Front,
    Back,
    FrontAndBack,
};

enum class FrontFace {
    CounterClockwise,
    Clockwise,
};

struct RasterizationState {
    bool depthClampEnable = false;
    bool rasterizerDiscardEnable = false;
    PolygonMode polygonMode = PolygonMode::Fill;
    CullMode cullMode = CullMode::Back;
    FrontFace frontFace = FrontFace::CounterClockwise;
    bool depthBiasEnable = false;
    float depthBiasConstantFactor = 0.0f;
    float depthBiasClamp = 0.0f;
    float depthBiasSlopeFactor = 0.0f;
    float lineWidth = 1.0f;
};

// ============================================================================
// Multisample
// ============================================================================

struct MultisampleState {
    uint32_t rasterizationSamples = 1;
    bool sampleShadingEnable = false;
    float minSampleShading = 1.0f;
    const uint32_t* sampleMask = nullptr;
    bool alphaToCoverageEnable = false;
    bool alphaToOneEnable = false;
};

// ============================================================================
// Depth/Stencil
// ============================================================================

enum class StencilOp {
    Keep,
    Zero,
    Replace,
    IncrementAndClamp,
    DecrementAndClamp,
    Invert,
    IncrementAndWrap,
    DecrementAndWrap,
};

struct StencilOpState {
    StencilOp failOp = StencilOp::Keep;
    StencilOp passOp = StencilOp::Keep;
    StencilOp depthFailOp = StencilOp::Keep;
    CompareOp compareOp = CompareOp::Always;
    uint32_t compareMask = 0xFF;
    uint32_t writeMask = 0xFF;
    uint32_t reference = 0;
};

struct DepthStencilState {
    bool depthTestEnable = true;
    bool depthWriteEnable = true;
    CompareOp depthCompareOp = CompareOp::Less;
    bool depthBoundsTestEnable = false;
    bool stencilTestEnable = false;
    StencilOpState front;
    StencilOpState back;
    float minDepthBounds = 0.0f;
    float maxDepthBounds = 1.0f;
};

// ============================================================================
// Color Blend
// ============================================================================

enum class BlendFactor {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate,
};

enum class BlendOp {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

enum class ColorComponentFlags : uint32_t {
    R = 1 << 0,
    G = 1 << 1,
    B = 1 << 2,
    A = 1 << 3,
    All = R | G | B | A,
};

struct ColorBlendAttachment {
    bool blendEnable = false;
    BlendFactor srcColorBlendFactor = BlendFactor::One;
    BlendFactor dstColorBlendFactor = BlendFactor::Zero;
    BlendOp colorBlendOp = BlendOp::Add;
    BlendFactor srcAlphaBlendFactor = BlendFactor::One;
    BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
    BlendOp alphaBlendOp = BlendOp::Add;
    ColorComponentFlags colorWriteMask = ColorComponentFlags::All;
};

struct ColorBlendState {
    bool logicOpEnable = false;
    // LogicOp logicOp = LogicOp::Copy;
    std::span<const ColorBlendAttachment> attachments;
    float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};

// ============================================================================
// Dynamic State
// ============================================================================

enum class DynamicState {
    Viewport,
    Scissor,
    LineWidth,
    DepthBias,
    BlendConstants,
    DepthBounds,
    StencilCompareMask,
    StencilWriteMask,
    StencilReference,
};

// ============================================================================
// Pipeline Descriptor
// ============================================================================

struct GraphicsPipelineDesc {
    // Shader stages
    Shader* vertexShader = nullptr;
    Shader* fragmentShader = nullptr;
    Shader* geometryShader = nullptr;
    Shader* tessControlShader = nullptr;
    Shader* tessEvalShader = nullptr;
    
    // Fixed-function state
    VertexInputState vertexInput;
    InputAssemblyState inputAssembly;
    RasterizationState rasterization;
    MultisampleState multisample;
    DepthStencilState depthStencil;
    ColorBlendState colorBlend;
    
    // Dynamic state
    std::span<const DynamicState> dynamicStates;
    
    // Render pass compatibility
    RenderPass* renderPass = nullptr;
    uint32_t subpass = 0;
    
    // Optional debug name
    const char* debugName = nullptr;
};

struct ComputePipelineDesc {
    Shader* computeShader = nullptr;
    
    // Optional debug name
    const char* debugName = nullptr;
};

struct PipelineDesc {
    PipelineType type = PipelineType::Graphics;
    
    union {
        GraphicsPipelineDesc graphics;
        ComputePipelineDesc compute;
    };
    
    PipelineDesc() : graphics{} {}
};

// ============================================================================
// Pipeline Interface
// ============================================================================

/// Graphics or compute pipeline interface
class Pipeline {
public:
    virtual ~Pipeline() = default;
    
    // Pipeline cannot be copied, only moved
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&&) noexcept = default;
    Pipeline& operator=(Pipeline&&) noexcept = default;
    
    // ========================================================================
    // Pipeline Information
    // ========================================================================
    
    /// Get pipeline type
    virtual PipelineType GetType() const noexcept = 0;
    
    // ========================================================================
    // Native Handle Access (Optional)
    // ========================================================================
    
    /// Get native pipeline handle for interoperability
    /// @return Native handle or nullptr if not supported
    virtual void* GetNativeHandle() const noexcept { return nullptr; }
    
protected:
    Pipeline() = default;
};

// Convenience alias for the public API


} // namespace VRHI
