#include "function/render/shader_compiler/glsl_compiler.h"
#include "function/render/shader_compiler/glsl_includer.h"
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Zafkiel 
{

static constexpr uint32_t SetStride = 16;

ScopedBuffer GlslCompiler::Compile(const std::string &source, GraphicsAPI api, ShaderStage stage, const std::string &name)
{
    shaderc_shader_kind kind = stage == ShaderStage::Vertex ? shaderc_glsl_vertex_shader
                             : stage == ShaderStage::Fragment ? shaderc_glsl_fragment_shader
                             : stage == ShaderStage::Geometry ? shaderc_glsl_geometry_shader 
                             : shaderc_glsl_vertex_shader;
    if (api == GraphicsAPI::OpenGL)
    {
        auto vulkanSpv = CompileVulkanSpv(source, name, kind);
        return CompileOpenGLSpv(vulkanSpv, name, kind);
    }      
    else if (api == GraphicsAPI::Vulkan)
    {
        return CompileVulkanSpv(source, name, kind);
    }
    return nullptr;
}

ScopedBuffer GlslCompiler::CompileVulkanSpv(const std::string &shaderCode, const std::string &shaderName, shaderc_shader_kind kind)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetOptimizationLevel(shaderc_optimization_level_zero);
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    options.SetSourceLanguage(shaderc_source_language_glsl);
    options.SetGenerateDebugInfo();
    options.SetIncluder(std::make_unique<GlslIncluder>(rootPath));

    const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
        shaderCode,
        kind,
        shaderName.c_str(),
        options 
    );

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        Log::Error("Failed to Compile Vulkan Shader! Error: {}", result.GetErrorMessage());
    }
    const uint8_t *data = (const uint8_t *)result.cbegin();
    uint32_t size = (result.cend() - result.cbegin()) * sizeof(uint32_t);

    return ScopedBuffer(data, size);
}

ScopedBuffer GlslCompiler::CompileOpenGLSpv(Buffer vulkanSpv, const std::string &shaderName, shaderc_shader_kind kind)
{
    spirv_cross::CompilerGLSL glslCompiler(vulkanSpv.Data<uint32_t>(), vulkanSpv.Size<uint32_t>());

    // binding映射
    auto resources = glslCompiler.get_shader_resources();

    for (auto& ubo : resources.uniform_buffers) {
        uint32_t set = glslCompiler.get_decoration(ubo.id, spv::DecorationDescriptorSet);
        uint32_t binding = glslCompiler.get_decoration(ubo.id, spv::DecorationBinding);

        uint32_t gl_binding = set * SetStride + binding;
        glslCompiler.set_decoration(ubo.id, spv::DecorationBinding, gl_binding);
    }

    for (auto& sampled_image : resources.sampled_images) {
        uint32_t set = glslCompiler.get_decoration(sampled_image.id, spv::DecorationDescriptorSet);
        uint32_t binding = glslCompiler.get_decoration(sampled_image.id, spv::DecorationBinding);

        uint32_t gl_binding = set * SetStride + binding;
        glslCompiler.set_decoration(sampled_image.id, spv::DecorationBinding, gl_binding);
    }

    std::string glslShaderCode = glslCompiler.compile();

    shaderc::Compiler compiler;
    shaderc::CompileOptions shadercOptions;

    shadercOptions.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
    shadercOptions.SetSourceLanguage(shaderc_source_language_glsl);
    shadercOptions.SetOptimizationLevel(shaderc_optimization_level_zero);
    shadercOptions.SetGenerateDebugInfo();

    const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
        glslShaderCode,
        kind,
        shaderName.c_str(),
        shadercOptions
    );
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        Log::Error("Failed to Compile Vulkan Shader! Error: {}", result.GetErrorMessage());
    }

    const uint8_t *data = (const uint8_t *)result.cbegin();
    uint32_t size = (result.cend() - result.cbegin()) * sizeof(uint32_t);

    return ScopedBuffer(data, size);
}

}