// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <VRHI/CommandBuffer.hpp>
#include <VRHI/Pipeline.hpp>
#include <VRHI/Resources.hpp>
#include <VRHI/Shader.hpp>
#include <VRHI/VRHI.hpp>
#include <VRHI/Window.hpp>

int main() {
    try {
        std::cout << "VRHI HelloTriangle Example\n";
        std::cout << "===========================\n\n";

        // Create window
        std::cout << "Creating window...\n";
        VRHI::WindowConfig windowConfig{};
        windowConfig.title = "VRHI - Hello Triangle";
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

        // Create vertex buffer
        std::cout << "Creating vertex buffer...\n";

        // Triangle vertices (position + color)
        // Format: x, y, z, r, g, b
        float vertices[] = {
            // Position          // Color
            0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // Top (Red)
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom-left (Green)
            0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom-right (Blue)
        };

        VRHI::BufferDesc bufferDesc{};
        bufferDesc.size = sizeof(vertices);
        bufferDesc.usage = VRHI::BufferUsage::Vertex;
        bufferDesc.initialData = vertices;

        auto bufferResult = device->CreateBuffer(bufferDesc);
        if (!bufferResult) {
            std::cerr << "Failed to create vertex buffer: " << bufferResult.error().message << "\n";
            return 1;
        }
        auto vertexBuffer = std::move(*bufferResult);
        std::cout << "Vertex buffer created\n";

        // Create shaders
        std::cout << "Creating shaders...\n";

        // Simple vertex shader
        const char* vertexShaderSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aColor;

            out vec3 vertexColor;

            void main() {
                gl_Position = vec4(aPos, 1.0);
                vertexColor = aColor;
            }
        )";

        // Simple fragment shader
        const char* fragmentShaderSource = R"(
            #version 330 core
            in vec3 vertexColor;
            out vec4 FragColor;

            void main() {
                FragColor = vec4(vertexColor, 1.0);
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

        VRHI::PipelineDesc pipelineDesc{};
        pipelineDesc.type = VRHI::PipelineType::Graphics;
        pipelineDesc.graphics.vertexShader = vertexShader.get();
        pipelineDesc.graphics.fragmentShader = fragmentShader.get();

        // Vertex input layout
        // Note: For simplicity, we're not setting up detailed vertex layout here
        // The OpenGL backend will infer it from the shader

        // Rasterization state
        pipelineDesc.graphics.rasterization.cullMode = VRHI::CullMode::None;
        pipelineDesc.graphics.rasterization.frontFace = VRHI::FrontFace::CounterClockwise;

        auto pipelineResult = device->CreatePipeline(pipelineDesc);
        if (!pipelineResult) {
            std::cerr << "Failed to create pipeline: " << pipelineResult.error().message << "\n";
            return 1;
        }
        auto pipeline = std::move(*pipelineResult);
        std::cout << "Pipeline created\n\n";

        std::cout << "Starting render loop...\n";
        std::cout << "Press ESC or close window to exit\n\n";

        // Render loop
        while (!window->ShouldClose()) {
            // Poll events
            window->PollEvents();

            // Create command buffer
            auto cmd = device->CreateCommandBuffer();

            // Begin command buffer
            cmd->Begin();

            // Clear screen
            VRHI::ClearColorValue clearColor;
            clearColor.float32[0] = 0.1f;  // R
            clearColor.float32[1] = 0.1f;  // G
            clearColor.float32[2] = 0.1f;  // B
            clearColor.float32[3] = 1.0f;  // A
            
            VRHI::Rect2D clearRect{};
            clearRect.x = 0;
            clearRect.y = 0;
            clearRect.width = window->GetWidth();
            clearRect.height = window->GetHeight();
            cmd->ClearColorAttachment(0, clearColor, clearRect);

            // Set viewport and scissor
            VRHI::Viewport viewport{};
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = static_cast<float>(window->GetWidth());
            viewport.height = static_cast<float>(window->GetHeight());
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            cmd->SetViewport(viewport);

            VRHI::Rect2D scissor{};
            scissor.x = 0;
            scissor.y = 0;
            scissor.width = window->GetWidth();
            scissor.height = window->GetHeight();
            cmd->SetScissor(scissor);

            // Bind pipeline and draw triangle
            cmd->BindPipeline(pipeline.get());

            // Bind vertex buffer
            VRHI::Buffer* buffers[] = {vertexBuffer.get()};
            uint64_t offsets[] = {0};
            cmd->BindVertexBuffers(0, buffers, offsets);

            cmd->Draw(3, 1, 0, 0);

            // End command buffer
            cmd->End();

            // Submit and present
            device->Submit(std::move(cmd));
            
            // Swap buffers to display the frame
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
