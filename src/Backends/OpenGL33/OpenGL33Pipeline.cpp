// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Pipeline.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

std::expected<std::unique_ptr<Pipeline>, Error>
OpenGL33Pipeline::Create(const PipelineDesc& desc) {
    // For OpenGL 3.3, we need to create a shader program
    GLuint program = glCreateProgram();
    
    if (program == 0) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to create OpenGL program"
        });
    }
    
    // Simplified implementation - in a real implementation, we would:
    // 1. Attach shaders from desc
    // 2. Link the program
    // 3. Validate the program
    // 4. Set up pipeline state
    
    // For now, just return a stub program
    return std::unique_ptr<Pipeline>(new OpenGL33Pipeline(program, desc.type));
}

} // namespace VRHI
