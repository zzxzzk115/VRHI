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
        
        // Display window and framebuffer dimensions (for HiDPI awareness)
        float xscale = 1.0f, yscale = 1.0f;
        window->GetContentScale(&xscale, &yscale);
        std::cout << "Window size: " << window->GetWidth() << "x" << window->GetHeight() << "\n";
        std::cout << "Framebuffer size: " << window->GetFramebufferWidth() << "x" << window->GetFramebufferHeight() << "\n";
        std::cout << "Content scale: " << xscale << "x" << yscale << "\n\n";

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
        std::cout << "Vendor: " << backendInfo.vendorName << "\n";

        // Print feature support
        std::cout << "\n=== Feature Support ===\n";
        const auto& features = device->GetFeatures();
        
        std::cout << "\nCore Features:\n";
        std::cout << "  Vertex Shader:       " << (features.core.vertexShader ? "✓" : "✗") << "\n";
        std::cout << "  Fragment Shader:     " << (features.core.fragmentShader ? "✓" : "✗") << "\n";
        std::cout << "  Geometry Shader:     " << (features.core.geometryShader ? "✓" : "✗") << "\n";
        std::cout << "  Tessellation Shader: " << (features.core.tessellationShader ? "✓" : "✗") << "\n";
        std::cout << "  Compute Shader:      " << (features.core.computeShader ? "✓" : "✗") << "\n";
        std::cout << "  Mesh Shader:         " << (features.core.meshShader ? "✓" : "✗") << "\n";
        std::cout << "  Instancing:          " << (features.core.instancing ? "✓" : "✗") << "\n";
        std::cout << "  Multi Draw Indirect: " << (features.core.multiDrawIndirect ? "✓" : "✗") << "\n";
        
        std::cout << "\nBuffer Features:\n";
        std::cout << "  Vertex Buffers:   " << (features.core.vertexBuffers ? "✓" : "✗") << "\n";
        std::cout << "  Index Buffers:    " << (features.core.indexBuffers ? "✓" : "✗") << "\n";
        std::cout << "  Uniform Buffers:  " << (features.core.uniformBuffers ? "✓" : "✗") << "\n";
        std::cout << "  Storage Buffers:  " << (features.core.storageBuffers ? "✓" : "✗") << "\n";
        std::cout << "  Indirect Buffers: " << (features.core.indirectBuffers ? "✓" : "✗") << "\n";
        
        std::cout << "\nTexture Features:\n";
        std::cout << "  Texture 1D:              " << (features.texture.texture1D ? "✓" : "✗") << "\n";
        std::cout << "  Texture 2D:              " << (features.texture.texture2D ? "✓" : "✗") << "\n";
        std::cout << "  Texture 3D:              " << (features.texture.texture3D ? "✓" : "✗") << "\n";
        std::cout << "  Texture Cube:            " << (features.texture.textureCube ? "✓" : "✗") << "\n";
        std::cout << "  Texture 2D Array:        " << (features.texture.texture2DArray ? "✓" : "✗") << "\n";
        std::cout << "  Float Textures:          " << (features.texture.floatTextures ? "✓" : "✗") << "\n";
        std::cout << "  Depth Textures:          " << (features.texture.depthTextures ? "✓" : "✗") << "\n";
        std::cout << "  Compressed Textures:     " << (features.texture.compressedTextures ? "✓" : "✗") << "\n";
        std::cout << "  Anisotropic Filtering:   " << (features.texture.anisotropicFiltering ? "✓" : "✗") << "\n";
        std::cout << "  Max Texture Size:        " << features.texture.maxTextureSize << "\n";
        std::cout << "  Max 3D Texture Size:     " << features.texture.max3DTextureSize << "\n";
        std::cout << "  Max Array Layers:        " << features.texture.maxArrayLayers << "\n";
        std::cout << "  Max Anisotropy:          " << features.texture.maxAnisotropy << "x\n";
        
        std::cout << "\nTexture Compression Formats:\n";
        std::cout << "  DXT/BC:  " << (features.texture.dxt ? "✓" : "✗") << "\n";
        std::cout << "  ETC2:    " << (features.texture.etc2 ? "✓" : "✗") << "\n";
        std::cout << "  ASTC:    " << (features.texture.astc ? "✓" : "✗") << "\n";
        
        std::cout << "\nRendering Features:\n";
        std::cout << "  Multiple Render Targets: " << (features.rendering.multipleRenderTargets ? "✓" : "✗") << "\n";
        std::cout << "  Max Color Attachments:   " << features.rendering.maxColorAttachments << "\n";
        std::cout << "  Independent Blend:       " << (features.rendering.independentBlend ? "✓" : "✗") << "\n";
        std::cout << "  Depth Clamp:             " << (features.rendering.depthClamp ? "✓" : "✗") << "\n";
        std::cout << "  Multisample:             " << (features.rendering.multisample ? "✓" : "✗") << "\n";
        std::cout << "  Max Samples:             " << features.rendering.maxSamples << "\n";
        
        std::cout << "\nCompute Features:\n";
        std::cout << "  Compute Shader:            " << (features.compute.computeShader ? "✓" : "✗") << "\n";
        if (features.compute.computeShader) {
            std::cout << "  Max Work Group Size X:     " << features.compute.maxWorkGroupSizeX << "\n";
            std::cout << "  Max Work Group Size Y:     " << features.compute.maxWorkGroupSizeY << "\n";
            std::cout << "  Max Work Group Size Z:     " << features.compute.maxWorkGroupSizeZ << "\n";
            std::cout << "  Max Work Group Invocations: " << features.compute.maxWorkGroupInvocations << "\n";
            std::cout << "  Max Shared Memory Size:    " << features.compute.maxComputeSharedMemorySize << " bytes\n";
        }
        
        std::cout << "\nAdvanced Features:\n";
        std::cout << "  Ray Tracing:          " << (features.advanced.rayTracing ? "✓" : "✗") << "\n";
        std::cout << "  Mesh Shading:         " << (features.advanced.meshShading ? "✓" : "✗") << "\n";
        std::cout << "  Variable Rate Shading: " << (features.advanced.variableRateShading ? "✓" : "✗") << "\n";
        std::cout << "  Bindless Resources:   " << (features.advanced.bindlessResources ? "✓" : "✗") << "\n";
        std::cout << "  Async Compute:        " << (features.advanced.asyncCompute ? "✓" : "✗") << "\n";
        
        std::cout << "\nMemory Features:\n";
        if (features.memory.deviceLocalMemory > 0) {
            std::cout << "  Device Local Memory:  " << (features.memory.deviceLocalMemory / 1024 / 1024) << " MB\n";
        } else {
            std::cout << "  Device Local Memory:  Unknown\n";
        }
        if (features.memory.hostVisibleMemory > 0) {
            std::cout << "  Host Visible Memory:  " << (features.memory.hostVisibleMemory / 1024 / 1024) << " MB\n";
        } else {
            std::cout << "  Host Visible Memory:  Unknown\n";
        }
        std::cout << "  Unified Memory:       " << (features.memory.unifiedMemory ? "✓" : "✗") << "\n";
        if (features.memory.minUniformBufferAlignment > 0) {
            std::cout << "  Min Uniform Buffer Alignment: " << features.memory.minUniformBufferAlignment << " bytes\n";
        }
        if (features.memory.minStorageBufferAlignment > 0) {
            std::cout << "  Min Storage Buffer Alignment: " << features.memory.minStorageBufferAlignment << " bytes\n";
        }
        
        // Test individual feature queries
        std::cout << "\n=== Feature Query Tests ===\n";
        std::cout << "IsFeatureSupported(Feature::Compute): " 
                  << (device->IsFeatureSupported(VRHI::Feature::Compute) ? "✓" : "✗") << "\n";
        std::cout << "IsFeatureSupported(Feature::Geometry): " 
                  << (device->IsFeatureSupported(VRHI::Feature::Geometry) ? "✓" : "✗") << "\n";
        std::cout << "IsFeatureSupported(Feature::Tessellation): " 
                  << (device->IsFeatureSupported(VRHI::Feature::Tessellation) ? "✓" : "✗") << "\n";
        std::cout << "IsFeatureSupported(Feature::Texture3D): " 
                  << (device->IsFeatureSupported(VRHI::Feature::Texture3D) ? "✓" : "✗") << "\n";
        std::cout << "IsFeatureSupported(Feature::AnisotropicFiltering): " 
                  << (device->IsFeatureSupported(VRHI::Feature::AnisotropicFiltering) ? "✓" : "✗") << "\n";
        std::cout << "IsFeatureSupported(Feature::RayTracing): " 
                  << (device->IsFeatureSupported(VRHI::Feature::RayTracing) ? "✓" : "✗") << "\n";
        
        std::cout << "\n======================\n\n";

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
            #version 460
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aColor;

            layout (location = 0) out vec3 vertexColor;

            void main() {
                gl_Position = vec4(aPos, 1.0);
                vertexColor = aColor;
            }
        )";

        // Simple fragment shader
        const char* fragmentShaderSource = R"(
            #version 460
            layout (location = 0) in vec3 vertexColor;
            layout (location = 0) out vec4 FragColor;

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

        // Define vertex layout (position + color)
        VRHI::VertexAttribute vertexAttributes[] = {
            {
                .location = 0,
                .binding = 0,
                .format = VRHI::VertexFormat::Float3,  // Position (x, y, z)
                .offset = 0
            },
            {
                .location = 1,
                .binding = 0,
                .format = VRHI::VertexFormat::Float3,  // Color (r, g, b)
                .offset = 3 * sizeof(float)
            }
        };
        
        VRHI::VertexBinding vertexBindings[] = {
            {
                .binding = 0,
                .stride = 6 * sizeof(float),  // Position (3 floats) + Color (3 floats)
                .inputRate = VRHI::VertexInputRate::Vertex
            }
        };

        VRHI::PipelineDesc pipelineDesc{};
        pipelineDesc.type = VRHI::PipelineType::Graphics;
        pipelineDesc.graphics.vertexShader = vertexShader.get();
        pipelineDesc.graphics.fragmentShader = fragmentShader.get();

        // Set vertex input layout
        pipelineDesc.graphics.vertexInput.attributes = vertexAttributes;
        pipelineDesc.graphics.vertexInput.bindings = vertexBindings;

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
            VRHI::ClearColorValue clearColor{};
            clearColor.float32[0] = 0.1f;  // R
            clearColor.float32[1] = 0.1f;  // G
            clearColor.float32[2] = 0.1f;  // B
            clearColor.float32[3] = 1.0f;  // A
            
            // Use framebuffer size for rendering (accounts for HiDPI/Retina displays)
            uint32_t fbWidth = window->GetFramebufferWidth();
            uint32_t fbHeight = window->GetFramebufferHeight();
            
            VRHI::Rect2D clearRect{};
            clearRect.x = 0;
            clearRect.y = 0;
            clearRect.width = fbWidth;
            clearRect.height = fbHeight;
            cmd->ClearColorAttachment(0, clearColor, clearRect);

            // Set viewport and scissor using framebuffer dimensions
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
