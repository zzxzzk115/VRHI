// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/ShaderCompiler.hpp>
#include <VRHI/Backend.hpp>
#include <VRHI/Logging.hpp>

#include <cstring>

// glslang includes
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>

// SPIRV-Cross includes
#include <spirv_glsl.hpp>

#include <mutex>

namespace VRHI {

// ============================================================================
// glslang Includer Implementation
// ============================================================================

namespace {

/// Wrapper around IShaderIncluder for glslang's TShader::Includer interface
class GlslangIncluder : public glslang::TShader::Includer {
public:
    explicit GlslangIncluder(IShaderIncluder* includer) : m_includer(includer) {}
    
    IncludeResult* includeLocal(const char* headerName,
                                const char* includerName,
                                size_t inclusionDepth) override {
        return includeSystem(headerName, includerName, inclusionDepth);
    }
    
    IncludeResult* includeSystem(const char* headerName,
                                 const char* includerName,
                                 size_t inclusionDepth) override {
        if (!m_includer) {
            return nullptr;
        }
        
        std::string content = m_includer->ResolveInclude(
            headerName,
            includerName,
            inclusionDepth
        );
        
        if (content.empty()) {
            return nullptr;
        }
        
        // Allocate storage for the included content
        char* data = new char[content.size()];
        std::memcpy(data, content.data(), content.size());
        
        return new IncludeResult(headerName, data, content.size(), nullptr);
    }
    
    void releaseInclude(IncludeResult* result) override {
        if (result) {
            delete[] static_cast<const char*>(result->headerData);
            delete result;
        }
    }
    
private:
    IShaderIncluder* m_includer;
};

} // anonymous namespace (for GlslangIncluder)

// ============================================================================
// glslang Initialization
// ============================================================================

namespace {
    std::once_flag g_glslangInitFlag;
    
    void InitializeGlslang() {
        std::call_once(g_glslangInitFlag, []() {
            glslang::InitializeProcess();
        });
    }
    
    // Convert VRHI shader stage to glslang stage
    EShLanguage GetGlslangShaderStage(ShaderStage stage) {
        switch (stage) {
            case ShaderStage::Vertex:      return EShLangVertex;
            case ShaderStage::Fragment:    return EShLangFragment;
            case ShaderStage::Geometry:    return EShLangGeometry;
            case ShaderStage::TessControl: return EShLangTessControl;
            case ShaderStage::TessEval:    return EShLangTessEvaluation;
            case ShaderStage::Compute:     return EShLangCompute;
            case ShaderStage::Mesh:        return EShLangMesh;
            case ShaderStage::Task:        return EShLangTask;
            // Ray tracing stages
            case ShaderStage::RayGeneration: return EShLangRayGen;
            case ShaderStage::AnyHit:        return EShLangAnyHit;
            case ShaderStage::ClosestHit:    return EShLangClosestHit;
            case ShaderStage::Miss:          return EShLangMiss;
            case ShaderStage::Intersection:  return EShLangIntersect;
            case ShaderStage::Callable:      return EShLangCallable;
            default:
                return EShLangVertex;
        }
    }

} // anonymous namespace

// ============================================================================
// Shader Compiler Implementation
// ============================================================================

std::expected<std::vector<uint32_t>, Error> 
ShaderCompiler::CompileGLSLToSPIRV(
    const std::string& source,
    ShaderStage stage,
    const char* entryPoint,
    IShaderIncluder* includer
) {
    InitializeGlslang();
    
    EShLanguage shaderStage = GetGlslangShaderStage(stage);
    glslang::TShader shader(shaderStage);
    
    // Set shader source
    const char* sourceStrings[] = { source.c_str() };
    const int sourceLengths[] = { static_cast<int>(source.length()) };
    shader.setStringsWithLengths(sourceStrings, sourceLengths, 1);
    
    // Set entry point
    shader.setEntryPoint(entryPoint);
    shader.setSourceEntryPoint(entryPoint);
    
    // Set GLSL version and target environment
    shader.setEnvInput(glslang::EShSourceGlsl, shaderStage, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);
    
    // Parse shader with optional includer support
    const TBuiltInResource* resources = GetDefaultResources();
    bool parseResult = false;
    
    if (includer) {
        GlslangIncluder glslangIncluder(includer);
        parseResult = shader.parse(resources, 100, false, EShMsgDefault, glslangIncluder);
    } else {
        parseResult = shader.parse(resources, 100, false, EShMsgDefault);
    }
    
    if (!parseResult) {
        std::string errorMsg = "Failed to parse GLSL shader:\n";
        errorMsg += shader.getInfoLog();
        errorMsg += shader.getInfoDebugLog();
        
        LogError(errorMsg);
        return std::unexpected(Error{Error::Code::ShaderCompilationFailed, errorMsg});
    }
    
    // Link shader
    glslang::TProgram program{};
    program.addShader(&shader);
    
    if (!program.link(EShMsgDefault)) {
        std::string errorMsg = "Failed to link shader program:\n";
        errorMsg += program.getInfoLog();
        errorMsg += program.getInfoDebugLog();
        
        LogError(errorMsg);
        return std::unexpected(Error{Error::Code::ShaderCompilationFailed, errorMsg});
    }
    
    // Convert to SPIR-V
    std::vector<uint32_t> spirv;
    glslang::SpvOptions spvOptions{};
    spvOptions.generateDebugInfo = false;
    spvOptions.disableOptimizer = true;
    spvOptions.optimizeSize = false;
    
    glslang::GlslangToSpv(*program.getIntermediate(shaderStage), spirv, &spvOptions);
    
    LogInfo("Successfully compiled GLSL to SPIR-V (%zu bytes)", 
            spirv.size() * sizeof(uint32_t));
    
    return spirv;
}

std::expected<std::string, Error>
ShaderCompiler::ConvertSPIRVToGLSL(
    std::span<const uint32_t> spirv,
    int targetVersion
) {
    try {
        // Create SPIRV-Cross compiler
        spirv_cross::CompilerGLSL compiler(spirv.data(), spirv.size());
        
        // Set GLSL version options
        spirv_cross::CompilerGLSL::Options options;
        options.version = targetVersion;
        options.es = false; // Desktop GLSL
        options.enable_420pack_extension = (targetVersion >= 420);
        
        compiler.set_common_options(options);
        
        // Compile SPIR-V to GLSL
        std::string glslSource = compiler.compile();
        
        LogInfo("Successfully converted SPIR-V to GLSL %d", targetVersion);
        
        return glslSource;
    } catch (const spirv_cross::CompilerError& e) {
        std::string errorMsg = "Failed to convert SPIR-V to GLSL: ";
        errorMsg += e.what();
        
        LogError(errorMsg);
        return std::unexpected(Error{Error::Code::ShaderCompilationFailed, errorMsg});
    }
}

std::expected<ShaderCompilationResult::ReflectionData, Error>
ShaderCompiler::ReflectSPIRV(std::span<const uint32_t> spirv) {
    try {
        // Create SPIRV-Cross compiler for reflection
        spirv_cross::CompilerGLSL compiler(spirv.data(), spirv.size());
        
        ShaderCompilationResult::ReflectionData reflection;
        
        // Get shader resources
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();
        
        // Reflect input attributes
        for (const auto& input : resources.stage_inputs) {
            reflection.inputs.push_back(input.name);
        }
        
        // Reflect output attributes
        for (const auto& output : resources.stage_outputs) {
            reflection.outputs.push_back(output.name);
        }
        
        // Reflect uniform buffers
        for (const auto& ubo : resources.uniform_buffers) {
            reflection.uniformBuffers.push_back(ubo.name);
        }
        
        // Reflect samplers
        for (const auto& sampler : resources.sampled_images) {
            reflection.samplers.push_back(sampler.name);
        }
        
        LogInfo("Reflected shader: %zu inputs, %zu outputs, %zu UBOs, %zu samplers",
                reflection.inputs.size(), reflection.outputs.size(),
                reflection.uniformBuffers.size(), reflection.samplers.size());
        
        return reflection;
    } catch (const spirv_cross::CompilerError& e) {
        std::string errorMsg = "Failed to reflect SPIR-V: ";
        errorMsg += e.what();
        
        LogError(errorMsg);
        return std::unexpected(Error{Error::Code::ShaderCompilationFailed, errorMsg});
    }
}

std::expected<ShaderCompilationResult, Error>
ShaderCompiler::CompileGLSL(
    const std::string& source,
    ShaderStage stage,
    const char* entryPoint,
    bool enableReflection,
    IShaderIncluder* includer
) {
    // Compile to SPIR-V
    auto spirvResult = CompileGLSLToSPIRV(source, stage, entryPoint, includer);
    if (!spirvResult) {
        return std::unexpected(spirvResult.error());
    }
    
    ShaderCompilationResult result{};
    result.spirv = std::move(*spirvResult);
    
    // Optionally generate reflection data
    if (enableReflection) {
        auto reflectionResult = ReflectSPIRV(result.spirv);
        if (!reflectionResult) {
            return std::unexpected(reflectionResult.error());
        }
        result.reflection = std::move(*reflectionResult);
    }
    
    return result;
}

} // namespace VRHI
