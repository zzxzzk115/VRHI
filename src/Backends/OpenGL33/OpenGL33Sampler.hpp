// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once
#include <expected>
#include <memory>

#include <VRHI/VRHI.hpp>
#include <VRHI/Resources.hpp>
#include <glad/glad.h>

namespace VRHI {

class OpenGL33Sampler : public Sampler {
public:
    ~OpenGL33Sampler() override;
    
    static std::expected<std::unique_ptr<Sampler>, Error>
    Create(const SamplerDesc& desc);
    
    GLuint GetHandle() const noexcept { return m_sampler; }
    
private:
    OpenGL33Sampler(GLuint sampler);
    
    GLuint m_sampler = 0;
};

} // namespace VRHI
