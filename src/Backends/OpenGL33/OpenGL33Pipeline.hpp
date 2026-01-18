// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once
#include <VRHI/VRHI.hpp>
#include <expected>
#include <memory>

#include <VRHI/Pipeline.hpp>
#include <glad/glad.h>

namespace VRHI {

class OpenGL33Pipeline : public Pipeline {
public:
    ~OpenGL33Pipeline() override;
    
    static std::expected<std::unique_ptr<Pipeline>, Error>
    Create(const PipelineDesc& desc);
    
    PipelineType GetType() const noexcept override { return m_type; }
    
    GLuint GetHandle() const noexcept { return m_program; }
    
    const VertexInputState& GetVertexInputState() const noexcept { return m_vertexInputState; }
    const DepthStencilState& GetDepthStencilState() const noexcept { return m_depthStencilState; }
    const RasterizationState& GetRasterizationState() const noexcept { return m_rasterizationState; }
    const ColorBlendState& GetColorBlendState() const noexcept { return m_colorBlendState; }
    
private:
    OpenGL33Pipeline(GLuint program, PipelineType type, const GraphicsPipelineDesc& desc);
    
    GLuint m_program = 0;
    PipelineType m_type = PipelineType::Graphics;
    
    // Store vertex layout for use during rendering
    VertexInputState m_vertexInputState;
    std::vector<VertexAttribute> m_vertexAttributes;
    std::vector<VertexBinding> m_vertexBindings;
    
    // Store pipeline state
    DepthStencilState m_depthStencilState;
    RasterizationState m_rasterizationState;
    ColorBlendState m_colorBlendState;
    std::vector<ColorBlendAttachment> m_colorBlendAttachments;
};

} // namespace VRHI
