// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <iostream>
#include <VRHI/VRHI.hpp>

void PrintFeatures(const VRHI::FeatureSet& features, const std::string& indentation = "") {
    std::cout << indentation << "Core Features:\n";
    std::cout << indentation << "  Vertex Shader:       " << (features.core.vertexShader ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Fragment Shader:     " << (features.core.fragmentShader ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Geometry Shader:     " << (features.core.geometryShader ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Tessellation Shader: " << (features.core.tessellationShader ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Compute Shader:      " << (features.core.computeShader ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Mesh Shader:         " << (features.core.meshShader ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Instancing:          " << (features.core.instancing ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Multi Draw Indirect: " << (features.core.multiDrawIndirect ? "✓" : "✗") << "\n";
    
    std::cout << indentation << "\nBuffer Features:\n";
    std::cout << indentation << "  Vertex Buffers:   " << (features.core.vertexBuffers ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Index Buffers:    " << (features.core.indexBuffers ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Uniform Buffers:  " << (features.core.uniformBuffers ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Storage Buffers:  " << (features.core.storageBuffers ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Indirect Buffers: " << (features.core.indirectBuffers ? "✓" : "✗") << "\n";
    
    std::cout << indentation << "\nTexture Features:\n";
    std::cout << indentation << "  Texture 1D:              " << (features.texture.texture1D ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Texture 2D:              " << (features.texture.texture2D ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Texture 3D:              " << (features.texture.texture3D ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Texture Cube:            " << (features.texture.textureCube ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Texture 2D Array:        " << (features.texture.texture2DArray ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Float Textures:          " << (features.texture.floatTextures ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Depth Textures:          " << (features.texture.depthTextures ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Compressed Textures:     " << (features.texture.compressedTextures ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Anisotropic Filtering:   " << (features.texture.anisotropicFiltering ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Max Texture Size:        " << features.texture.maxTextureSize << "\n";
    std::cout << indentation << "  Max 3D Texture Size:     " << features.texture.max3DTextureSize << "\n";
    std::cout << indentation << "  Max Array Layers:        " << features.texture.maxArrayLayers << "\n";
    std::cout << indentation << "  Max Anisotropy:          " << features.texture.maxAnisotropy << "x\n";
    
    std::cout << indentation << "\nTexture Compression Formats:\n";
    std::cout << indentation << "  DXT/BC:  " << (features.texture.dxt ? "✓" : "✗") << "\n";
    std::cout << indentation << "  ETC2:    " << (features.texture.etc2 ? "✓" : "✗") << "\n";
    std::cout << indentation << "  ASTC:    " << (features.texture.astc ? "✓" : "✗") << "\n";
    
    std::cout << indentation << "\nRendering Features:\n";
    std::cout << indentation << "  Multiple Render Targets: " << (features.rendering.multipleRenderTargets ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Max Color Attachments:   " << features.rendering.maxColorAttachments << "\n";
    std::cout << indentation << "  Independent Blend:       " << (features.rendering.independentBlend ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Depth Clamp:             " << (features.rendering.depthClamp ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Multisample:             " << (features.rendering.multisample ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Max Samples:             " << features.rendering.maxSamples << "\n";
    
    std::cout << indentation << "\nCompute Features:\n";
    std::cout << indentation << "  Compute Shader:            " << (features.compute.computeShader ? "✓" : "✗") << "\n";
    if (features.compute.computeShader) {
        std::cout << indentation << "  Max Work Group Size X:     " << features.compute.maxWorkGroupSizeX << "\n";
        std::cout << indentation << "  Max Work Group Size Y:     " << features.compute.maxWorkGroupSizeY << "\n";
        std::cout << indentation << "  Max Work Group Size Z:     " << features.compute.maxWorkGroupSizeZ << "\n";
        std::cout << indentation << "  Max Work Group Invocations: " << features.compute.maxWorkGroupInvocations << "\n";
        std::cout << indentation << "  Max Shared Memory Size:    " << features.compute.maxComputeSharedMemorySize << " bytes\n";
    }
    
    std::cout << indentation << "\nAdvanced Features:\n";
    std::cout << indentation << "  Ray Tracing:          " << (features.advanced.rayTracing ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Mesh Shading:         " << (features.advanced.meshShading ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Variable Rate Shading: " << (features.advanced.variableRateShading ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Bindless Resources:   " << (features.advanced.bindlessResources ? "✓" : "✗") << "\n";
    std::cout << indentation << "  Async Compute:        " << (features.advanced.asyncCompute ? "✓" : "✗") << "\n";
    
    std::cout << indentation << "\nMemory Features:\n";
    if (features.memory.deviceLocalMemory > 0) {
        std::cout << indentation << "  Device Local Memory:  " << (features.memory.deviceLocalMemory / 1024 / 1024) << " MB\n";
    } else {
        std::cout << indentation << "  Device Local Memory:  Unknown\n";
    }
    if (features.memory.hostVisibleMemory > 0) {
        std::cout << indentation << "  Host Visible Memory:  " << (features.memory.hostVisibleMemory / 1024 / 1024) << " MB\n";
    } else {
        std::cout << indentation << "  Host Visible Memory:  Unknown\n";
    }
    std::cout << indentation << "  Unified Memory:       " << (features.memory.unifiedMemory ? "✓" : "✗") << "\n";
    if (features.memory.minUniformBufferAlignment > 0) {
        std::cout << indentation << "  Min Uniform Buffer Alignment: " << features.memory.minUniformBufferAlignment << " bytes\n";
    }
    if (features.memory.minStorageBufferAlignment > 0) {
        std::cout << indentation << "  Min Storage Buffer Alignment: " << features.memory.minStorageBufferAlignment << " bytes\n";
    }
}

int main() {
    std::cout << "VRHI Feature Detection Example\n";
    std::cout << "================================\n\n";
    
    try {
        // Initialize VRHI
        VRHI::Initialize();
        std::cout << "VRHI initialized successfully\n\n";
        
        // Enumerate all available backends
        std::cout << "Enumerating backends...\n";
        auto backends = VRHI::EnumerateBackends();
        std::cout << "Found " << backends.size() << " backend(s)\n\n";
        
        for (size_t i = 0; i < backends.size(); ++i) {
            const auto& backend = backends[i];
            std::cout << "Backend #" << (i + 1) << ": " << backend.name << "\n";
            std::cout << "  Version: " << backend.version << "\n";
            std::cout << "  Score:   " << backend.score << "\n";
            std::cout << "  Type:    " << static_cast<int>(backend.type) << "\n\n";
            
            std::cout << "  Features:\n";
            PrintFeatures(backend.features, "    ");
            std::cout << "\n";
        }
        
        // Test feature requirement validation
        std::cout << "========================================\n";
        std::cout << "Testing Feature Requirement Validation\n";
        std::cout << "========================================\n\n";
        
        // Test 1: Create device without requirements
        std::cout << "Test 1: Create device without requirements\n";
        VRHI::DeviceConfig config1{};
        config1.preferredBackend = VRHI::BackendType::Auto;
        auto device1Result = VRHI::CreateDevice(config1);
        if (device1Result) {
            std::cout << "✓ Device created successfully\n";
            auto& device1 = device1Result.value();
            auto info = device1->GetBackendInfo();
            std::cout << "  Selected backend: " << info.name << "\n\n";
        } else {
            std::cout << "✗ Failed: " << device1Result.error().message << "\n\n";
        }
        
        // Test 2: Create device with basic requirements (should succeed for OpenGL33)
        std::cout << "Test 2: Create device with basic requirements\n";
        std::cout << "  Required: Texture2D, Texture3D\n";
        VRHI::DeviceConfig config2{};
        config2.preferredBackend = VRHI::BackendType::Auto;
        config2.features.required = {
            VRHI::Feature::Texture2D,
            VRHI::Feature::Texture3D
        };
        auto device2Result = VRHI::CreateDevice(config2);
        if (device2Result) {
            std::cout << "✓ Device created successfully\n";
            auto& device2 = device2Result.value();
            auto info = device2->GetBackendInfo();
            std::cout << "  Selected backend: " << info.name << "\n\n";
        } else {
            std::cout << "✗ Failed: " << device2Result.error().message << "\n\n";
        }
        
        // Test 3: Create device with unsupported requirement (should fail for OpenGL33)
        std::cout << "Test 3: Create device with unsupported requirement\n";
        std::cout << "  Required: Compute, RayTracing\n";
        VRHI::DeviceConfig config3{};
        config3.preferredBackend = VRHI::BackendType::Auto;
        config3.features.required = {
            VRHI::Feature::Compute,
            VRHI::Feature::RayTracing
        };
        auto device3Result = VRHI::CreateDevice(config3);
        if (device3Result) {
            std::cout << "✓ Device created successfully (unexpected!)\n";
            auto& device3 = device3Result.value();
            auto info = device3->GetBackendInfo();
            std::cout << "  Selected backend: " << info.name << "\n\n";
        } else {
            std::cout << "✓ Failed as expected: " << device3Result.error().message << "\n\n";
        }
        
        // Shutdown VRHI
        VRHI::Shutdown();
        std::cout << "VRHI shutdown successfully\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
}
