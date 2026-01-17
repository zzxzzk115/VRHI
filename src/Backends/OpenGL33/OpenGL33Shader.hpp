// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once
#include <expected>
#include <memory>

#include <VRHI/VRHI.hpp>
#include <VRHI/Shader.hpp>
#include <glad/glad.h>

namespace VRHI {

class OpenGL33Shader : public Shader {
public:
    ~OpenGL33Shader() override = default;
    
    static std::expected<std::unique_ptr<Shader>, Error>
    Create(const ShaderDesc& desc);
    
    ShaderStage GetStage() const noexcept override { return m_stage; }
    ShaderLanguage GetLanguage() const noexcept override { return m_language; }
    std::string_view GetEntryPoint() const noexcept override { return m_entryPoint; }
    
    GLuint GetHandle() const noexcept { return m_shader; }
    
private:
    OpenGL33Shader(GLuint shader, ShaderStage stage, ShaderLanguage language, std::string entryPoint)
        : m_shader(shader), m_stage(stage), m_language(language), m_entryPoint(std::move(entryPoint)) {}
    
    GLuint m_shader = 0;
    ShaderStage m_stage;
    ShaderLanguage m_language;
    std::string m_entryPoint;
};

} // namespace VRHI
