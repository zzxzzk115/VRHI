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
    ~OpenGL33Pipeline() override = default;
    
    static std::expected<std::unique_ptr<Pipeline>, Error>
    Create(const PipelineDesc& desc);
    
    PipelineType GetType() const noexcept override { return m_type; }
    
    GLuint GetHandle() const noexcept { return m_program; }
    
private:
    OpenGL33Pipeline(GLuint program, PipelineType type) : m_program(program), m_type(type) {}
    GLuint m_program = 0;
    PipelineType m_type = PipelineType::Graphics;
};

} // namespace VRHI
