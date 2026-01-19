// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>

#include <VRHI/CommandBuffer.hpp>
#include <VRHI/Pipeline.hpp>
#include <VRHI/Resources.hpp>
#include <VRHI/Shader.hpp>
#include <VRHI/VRHI.hpp>
#include <VRHI/Window.hpp>

// stb_image for texture loading (now properly integrated as a library)
#include <stb_image.h>

// OpenGL for setting sampler uniforms (temporary until API extended)
#include <glad/glad.h>
#include "../../src/Backends/OpenGL33/OpenGL33Pipeline.hpp"

// Simple matrix math for MVP transformations
struct Matrix4x4 {
    float m[16];
    
    static Matrix4x4 Identity() {
        Matrix4x4 mat = {};
        mat.m[0] = mat.m[5] = mat.m[10] = mat.m[15] = 1.0f;
        return mat;
    }
    
    static Matrix4x4 Perspective(float fovY, float aspect, float nearZ, float farZ) {
        Matrix4x4 mat = {};
        float tanHalfFovy = std::tan(fovY / 2.0f);
        
        // Column-major OpenGL perspective matrix
        mat.m[0] = 1.0f / (aspect * tanHalfFovy);
        mat.m[5] = 1.0f / tanHalfFovy;
        mat.m[10] = -(farZ + nearZ) / (farZ - nearZ);
        mat.m[14] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
        mat.m[11] = -1.0f;
        return mat;
    }
    
    static Matrix4x4 LookAt(float eyeX, float eyeY, float eyeZ,
                           float centerX, float centerY, float centerZ,
                           float upX, float upY, float upZ) {
        Matrix4x4 mat = {};
        
        // Forward vector (normalized)
        float fx = centerX - eyeX;
        float fy = centerY - eyeY;
        float fz = centerZ - eyeZ;
        float flen = std::sqrt(fx*fx + fy*fy + fz*fz);
        fx /= flen; fy /= flen; fz /= flen;
        
        // Right vector (normalized)
        float rx = fy * upZ - fz * upY;
        float ry = fz * upX - fx * upZ;
        float rz = fx * upY - fy * upX;
        float rlen = std::sqrt(rx*rx + ry*ry + rz*rz);
        rx /= rlen; ry /= rlen; rz /= rlen;
        
        // Up vector
        float ux = ry * fz - rz * fy;
        float uy = rz * fx - rx * fz;
        float uz = rx * fy - ry * fx;
        
        // Column-major OpenGL view matrix
        // Column 0: right vector
        mat.m[0] = rx;
        mat.m[1] = ry;
        mat.m[2] = rz;
        mat.m[3] = 0.0f;
        
        // Column 1: up vector
        mat.m[4] = ux;
        mat.m[5] = uy;
        mat.m[6] = uz;
        mat.m[7] = 0.0f;
        
        // Column 2: -forward vector
        mat.m[8] = -fx;
        mat.m[9] = -fy;
        mat.m[10] = -fz;
        mat.m[11] = 0.0f;
        
        // Column 3: translation
        mat.m[12] = -(rx*eyeX + ry*eyeY + rz*eyeZ);
        mat.m[13] = -(ux*eyeX + uy*eyeY + uz*eyeZ);
        mat.m[14] = fx*eyeX + fy*eyeY + fz*eyeZ;
        mat.m[15] = 1.0f;
        
        return mat;
    }
    
    static Matrix4x4 Rotate(float angle, float x, float y, float z) {
        Matrix4x4 mat = Identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        float t = 1.0f - c;
        
        // Normalize axis
        float len = std::sqrt(x*x + y*y + z*z);
        if (len > 0.0f) {
            x /= len; y /= len; z /= len;
        }
        
        // Column-major rotation matrix
        // Column 0
        mat.m[0] = t*x*x + c;
        mat.m[1] = t*x*y + s*z;
        mat.m[2] = t*x*z - s*y;
        mat.m[3] = 0.0f;
        
        // Column 1
        mat.m[4] = t*x*y - s*z;
        mat.m[5] = t*y*y + c;
        mat.m[6] = t*y*z + s*x;
        mat.m[7] = 0.0f;
        
        // Column 2
        mat.m[8] = t*x*z + s*y;
        mat.m[9] = t*y*z - s*x;
        mat.m[10] = t*z*z + c;
        mat.m[11] = 0.0f;
        
        // Column 3
        mat.m[12] = 0.0f;
        mat.m[13] = 0.0f;
        mat.m[14] = 0.0f;
        mat.m[15] = 1.0f;
        
        return mat;
    }
    
    static Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b) {
        Matrix4x4 result = {};
        // Column-major multiplication
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                result.m[col*4 + row] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result.m[col*4 + row] += a.m[k*4 + row] * b.m[col*4 + k];
                }
            }
        }
        return result;
    }
};

int main() {
    try {
        std::cout << "VRHI TexturedCube Example\n";
        std::cout << "=========================\n\n";

        // Create window
        std::cout << "Creating window...\n";
        VRHI::WindowConfig windowConfig{};
        windowConfig.title = "VRHI - Textured Cube";
        windowConfig.width = 800;
        windowConfig.height = 600;
        windowConfig.opengl.majorVersion = 3;
        windowConfig.opengl.minorVersion = 3;
        windowConfig.opengl.coreProfile = true;

        auto windowResult = VRHI::CreateWindow(windowConfig, VRHI::WindowSystemType::GLFW);
        if (!windowResult) {
            std::cerr << "Failed to create window: " << windowResult.error().message << "\n";
            return 1;
        }
        auto window = std::move(*windowResult);
        std::cout << "Window created successfully\n";

        // Create device
        std::cout << "Creating device...\n";
        VRHI::DeviceConfig deviceConfig{};
        deviceConfig.preferredBackend = VRHI::BackendType::OpenGL33;

        auto deviceResult = VRHI::CreateDevice(deviceConfig);
        if (!deviceResult) {
            std::cerr << "Failed to create device: " << deviceResult.error().message << "\n";
            return 1;
        }
        auto device = std::move(*deviceResult);
        std::cout << "Device created successfully\n";

        // Print device info
        auto backendInfo = device->GetBackendInfo();
        std::cout << "\nBackend: " << backendInfo.name << "\n";
        std::cout << "Device: " << backendInfo.deviceName << "\n";
        std::cout << "Vendor: " << backendInfo.vendorName << "\n\n";

        // Load texture
        std::cout << "Loading texture...\n";
        int texWidth, texHeight, texChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* pixels = stbi_load("assets/textures/awesomeface.png", &texWidth, &texHeight, &texChannels, 4);
        
        if (!pixels) {
            std::cerr << "Failed to load texture image\n";
            return 1;
        }
        
        std::cout << "Texture loaded: " << texWidth << "x" << texHeight << " (" << texChannels << " channels)\n";

        // Create texture
        VRHI::TextureDesc texDesc{};
        texDesc.type = VRHI::TextureType::Texture2D;
        texDesc.format = VRHI::TextureFormat::RGBA8_UNorm;
        texDesc.usage = VRHI::TextureUsage::Sampled;
        texDesc.width = texWidth;
        texDesc.height = texHeight;
        texDesc.mipLevels = 1;
        texDesc.initialData = pixels;

        auto textureResult = device->CreateTexture(texDesc);
        stbi_image_free(pixels);
        
        if (!textureResult) {
            std::cerr << "Failed to create texture: " << textureResult.error().message << "\n";
            return 1;
        }
        auto texture = std::move(*textureResult);
        std::cout << "Texture created\n";

        // Create sampler
        VRHI::SamplerDesc samplerDesc{};
        samplerDesc.minFilter = VRHI::FilterMode::Linear;
        samplerDesc.magFilter = VRHI::FilterMode::Linear;
        samplerDesc.mipmapMode = VRHI::FilterMode::Linear;
        samplerDesc.addressModeU = VRHI::AddressMode::Repeat;
        samplerDesc.addressModeV = VRHI::AddressMode::Repeat;
        samplerDesc.addressModeW = VRHI::AddressMode::Repeat;

        auto samplerResult = device->CreateSampler(samplerDesc);
        if (!samplerResult) {
            std::cerr << "Failed to create sampler: " << samplerResult.error().message << "\n";
            return 1;
        }
        auto sampler = std::move(*samplerResult);
        std::cout << "Sampler created\n";

        // Cube vertices (position + normal + UV)
        // Format: x, y, z, nx, ny, nz, u, v
        float vertices[] = {
            // Front face
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
            
            // Back face
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
            
            // Top face
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
            
            // Bottom face
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
            
            // Right face
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            
            // Left face
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        };

        // Cube indices
        uint16_t indices[] = {
            // Front
            0, 1, 2,  2, 3, 0,
            // Back
            4, 5, 6,  6, 7, 4,
            // Top
            8, 9, 10,  10, 11, 8,
            // Bottom
            12, 13, 14,  14, 15, 12,
            // Right
            16, 17, 18,  18, 19, 16,
            // Left
            20, 21, 22,  22, 23, 20,
        };

        // Create vertex buffer
        std::cout << "Creating vertex buffer...\n";
        VRHI::BufferDesc vbDesc{};
        vbDesc.size = sizeof(vertices);
        vbDesc.usage = VRHI::BufferUsage::Vertex;
        vbDesc.initialData = vertices;

        auto vertexBufferResult = device->CreateBuffer(vbDesc);
        if (!vertexBufferResult) {
            std::cerr << "Failed to create vertex buffer: " << vertexBufferResult.error().message << "\n";
            return 1;
        }
        auto vertexBuffer = std::move(*vertexBufferResult);
        std::cout << "Vertex buffer created\n";

        // Create index buffer
        std::cout << "Creating index buffer...\n";
        VRHI::BufferDesc ibDesc{};
        ibDesc.size = sizeof(indices);
        ibDesc.usage = VRHI::BufferUsage::Index;
        ibDesc.initialData = indices;

        auto indexBufferResult = device->CreateBuffer(ibDesc);
        if (!indexBufferResult) {
            std::cerr << "Failed to create index buffer: " << indexBufferResult.error().message << "\n";
            return 1;
        }
        auto indexBuffer = std::move(*indexBufferResult);
        std::cout << "Index buffer created\n";

        // Create uniform buffer for MVP matrix
        std::cout << "Creating uniform buffer...\n";
        VRHI::BufferDesc ubDesc{};
        ubDesc.size = sizeof(Matrix4x4);
        ubDesc.usage = VRHI::BufferUsage::Uniform;
        ubDesc.memoryAccess = VRHI::MemoryAccess::CpuToGpu;

        auto uniformBufferResult = device->CreateBuffer(ubDesc);
        if (!uniformBufferResult) {
            std::cerr << "Failed to create uniform buffer: " << uniformBufferResult.error().message << "\n";
            return 1;
        }
        auto uniformBuffer = std::move(*uniformBufferResult);
        std::cout << "Uniform buffer created\n";

        // Create shaders
        std::cout << "Creating shaders...\n";

        const char* vertexShaderSource = R"(
            #version 460
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;
            layout (location = 2) in vec2 aTexCoord;

            layout (location = 0) out vec3 FragPos;
            layout (location = 1) out vec3 Normal;
            layout (location = 2) out vec2 TexCoord;

            layout (std140, binding = 0) uniform UniformBufferObject {
                mat4 mvp;
            } ubo;

            void main() {
                gl_Position = ubo.mvp * vec4(aPos, 1.0);
                FragPos = aPos;
                Normal = aNormal;
                TexCoord = aTexCoord;
            }
        )";

        const char* fragmentShaderSource = R"(
            #version 460
            layout (location = 0) in vec3 FragPos;
            layout (location = 1) in vec3 Normal;
            layout (location = 2) in vec2 TexCoord;

            layout (location = 0) out vec4 FragColor;

            layout (binding = 0) uniform sampler2D texSampler;

            void main() {
                vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
                float diff = max(dot(Normal, lightDir), 0.3);
                vec4 texColor = texture(texSampler, TexCoord);
                FragColor = vec4(texColor.rgb * diff, texColor.a);
            }
        )";

        VRHI::ShaderDesc vertexShaderDesc{};
        vertexShaderDesc.stage = VRHI::ShaderStage::Vertex;
        vertexShaderDesc.code = vertexShaderSource;
        vertexShaderDesc.codeSize = std::strlen(vertexShaderSource);
        vertexShaderDesc.entryPoint = "main";

        auto vertexShaderResult = device->CreateShader(vertexShaderDesc);
        if (!vertexShaderResult) {
            std::cerr << "Failed to create vertex shader: " << vertexShaderResult.error().message << "\n";
            return 1;
        }
        auto vertexShader = std::move(*vertexShaderResult);

        VRHI::ShaderDesc fragmentShaderDesc{};
        fragmentShaderDesc.stage = VRHI::ShaderStage::Fragment;
        fragmentShaderDesc.code = fragmentShaderSource;
        fragmentShaderDesc.codeSize = std::strlen(fragmentShaderSource);
        fragmentShaderDesc.entryPoint = "main";

        auto fragmentShaderResult = device->CreateShader(fragmentShaderDesc);
        if (!fragmentShaderResult) {
            std::cerr << "Failed to create fragment shader: " << fragmentShaderResult.error().message << "\n";
            return 1;
        }
        auto fragmentShader = std::move(*fragmentShaderResult);
        std::cout << "Shaders created\n";

        // Create pipeline
        std::cout << "Creating pipeline...\n";

        VRHI::VertexAttribute vertexAttributes[] = {
            {
                .location = 0,
                .binding = 0,
                .format = VRHI::VertexFormat::Float3,  // Position
                .offset = 0
            },
            {
                .location = 1,
                .binding = 0,
                .format = VRHI::VertexFormat::Float3,  // Normal
                .offset = 3 * sizeof(float)
            },
            {
                .location = 2,
                .binding = 0,
                .format = VRHI::VertexFormat::Float2,  // TexCoord
                .offset = 6 * sizeof(float)
            }
        };
        
        VRHI::VertexBinding vertexBindings[] = {
            {
                .binding = 0,
                .stride = 8 * sizeof(float),  // 3 (pos) + 3 (normal) + 2 (uv)
                .inputRate = VRHI::VertexInputRate::Vertex
            }
        };

        VRHI::PipelineDesc pipelineDesc{};
        pipelineDesc.type = VRHI::PipelineType::Graphics;
        pipelineDesc.graphics.vertexShader = vertexShader.get();
        pipelineDesc.graphics.fragmentShader = fragmentShader.get();
        pipelineDesc.graphics.vertexInput.attributes = vertexAttributes;
        pipelineDesc.graphics.vertexInput.bindings = vertexBindings;
        pipelineDesc.graphics.rasterization.cullMode = VRHI::CullMode::Back;
        pipelineDesc.graphics.rasterization.frontFace = VRHI::FrontFace::CounterClockwise;
        pipelineDesc.graphics.depthStencil.depthTestEnable = true;
        pipelineDesc.graphics.depthStencil.depthWriteEnable = true;
        pipelineDesc.graphics.depthStencil.depthCompareOp = VRHI::CompareOp::Less;

        auto pipelineResult = device->CreatePipeline(pipelineDesc);
        if (!pipelineResult) {
            std::cerr << "Failed to create pipeline: " << pipelineResult.error().message << "\n";
            return 1;
        }
        auto pipeline = std::move(*pipelineResult);
        std::cout << "Pipeline created\n\n";

        // In OpenGL 3.3, we need to manually set the sampler uniform to use texture unit 1
        // since layout(binding = N) for samplers requires OpenGL 4.2+
        auto* glPipeline = static_cast<VRHI::OpenGL33Pipeline*>(pipeline.get());
        GLuint program = glPipeline->GetHandle();
        glUseProgram(program);
        GLint samplerLocation = glGetUniformLocation(program, "texSampler");
        if (samplerLocation >= 0) {
            glUniform1i(samplerLocation, 1);  // Bind to texture unit 1
            std::cout << "Sampler uniform set to texture unit 1\n";
        } else {
            std::cerr << "Warning: Could not find texSampler uniform\n";
        }

        std::cout << "Starting render loop...\n";
        std::cout << "Press ESC or close window to exit\n\n";

        // Timing
        auto startTime = std::chrono::high_resolution_clock::now();

        // Render loop
        while (!window->ShouldClose()) {
            window->PollEvents();

            // Calculate elapsed time
            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float>(currentTime - startTime).count();

            // Update MVP matrix
            float aspect = static_cast<float>(window->GetFramebufferWidth()) / 
                          static_cast<float>(window->GetFramebufferHeight());
            
            Matrix4x4 model = Matrix4x4::Rotate(time, 0.0f, 1.0f, 0.0f);
            Matrix4x4 view = Matrix4x4::LookAt(3.0f, 2.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            Matrix4x4 projection = Matrix4x4::Perspective(45.0f * 3.14159f / 180.0f, aspect, 0.1f, 100.0f);
            
            Matrix4x4 mvp = Matrix4x4::Multiply(projection, Matrix4x4::Multiply(view, model));
            uniformBuffer->Update(&mvp, sizeof(Matrix4x4), 0);

            // Create command buffer
            auto cmd = device->CreateCommandBuffer();
            cmd->Begin();

            // Clear screen and depth buffer
            VRHI::ClearColorValue clearColor{};
            clearColor.float32[0] = 0.2f;  // R
            clearColor.float32[1] = 0.3f;  // G
            clearColor.float32[2] = 0.3f;  // B
            clearColor.float32[3] = 1.0f;  // A
            
            uint32_t fbWidth = window->GetFramebufferWidth();
            uint32_t fbHeight = window->GetFramebufferHeight();
            
            VRHI::Rect2D clearRect{};
            clearRect.x = 0;
            clearRect.y = 0;
            clearRect.width = fbWidth;
            clearRect.height = fbHeight;
            cmd->ClearColorAttachment(0, clearColor, clearRect);
            
            VRHI::ClearDepthStencilValue depthStencilClear{};
            depthStencilClear.depth = 1.0f;
            depthStencilClear.stencil = 0;
            cmd->ClearDepthStencilAttachment(depthStencilClear, clearRect);

            // Set viewport and scissor
            VRHI::Viewport viewport{};
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = static_cast<float>(fbWidth);
            viewport.height = static_cast<float>(fbHeight);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            cmd->SetViewport(viewport);

            VRHI::Rect2D scissor{};
            scissor.x = 0;
            scissor.y = 0;
            scissor.width = fbWidth;
            scissor.height = fbHeight;
            cmd->SetScissor(scissor);

            // Bind pipeline
            cmd->BindPipeline(pipeline.get());
            
            // Bind uniform buffer and texture
            cmd->BindUniformBuffer(0, uniformBuffer.get());
            cmd->BindTexture(1, texture.get(), sampler.get());

            // Bind vertex and index buffers
            VRHI::Buffer* buffers[] = {vertexBuffer.get()};
            uint64_t offsets[] = {0};
            cmd->BindVertexBuffers(0, buffers, offsets);
            cmd->BindIndexBuffer(indexBuffer.get(), 0, true);  // 16-bit indices

            // Draw cube
            cmd->DrawIndexed(36, 1, 0, 0, 0);

            cmd->End();

            // Submit and present
            device->Submit(std::move(cmd));
            window->SwapBuffers();
        }

        // Wait for all operations to complete
        device->WaitIdle();

        std::cout << "\nExiting...\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
}
