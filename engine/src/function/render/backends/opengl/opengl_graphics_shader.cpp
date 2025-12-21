#include "opengl_graphics_shader.h"
#include "opengl_shader.h"
#include "opengl_vertex_module.h"
#include "opengl_fragment_module.h"
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <glad/glad.h>

namespace Zafkiel
{

static constexpr uint32_t SetStride = 16;

static std::tuple<std::string, std::string, std::string> ParseShader(const std::string &shaderCode)
{
    std::string vertexShaderCode, fragmentShaderCode, geometryShaderCode;
    const char *typeToken = "#type";
    auto typeTokenLength = strlen(typeToken);
    auto pos = shaderCode.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        auto eol = shaderCode.find_first_of("\r\n", pos);
        if (eol == std::string::npos)
        {
            Log::Error("Syntax error");
        }
        auto begin = pos + typeTokenLength + 1;
        std::string type = shaderCode.substr(begin, eol - begin);
        if (type != "vertex" && type != "fragment" && type != "geometry")
        {
            Log::Error("Invalid shader type");
        }
        auto nextLinePos = shaderCode.find_first_not_of("\r\n", eol);
        pos = shaderCode.find(typeToken, nextLinePos);
        if (type == "vertex")
            vertexShaderCode = shaderCode.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? shaderCode.size() - 1 : nextLinePos));
        else if (type == "fragment")
            fragmentShaderCode = shaderCode.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? shaderCode.size() - 1 : nextLinePos));
        else if (type == "geometry")
            geometryShaderCode = shaderCode.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? shaderCode.size() - 1 : nextLinePos));
    }
    if (vertexShaderCode.empty() || fragmentShaderCode.empty())
    {
        Log::Error("Incompleted shader source!");
    }
    return {vertexShaderCode, fragmentShaderCode, geometryShaderCode};
}

static ScopedBuffer CompileVulkanSpv(const std::string &shaderCode, const std::string &shaderName, shaderc_shader_kind kind)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    options.SetSourceLanguage(shaderc_source_language_glsl);
    options.SetGenerateDebugInfo();

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

static ScopedBuffer CompileOpenGLSpv(Buffer vulkanSpv, const std::string &shaderName, shaderc_shader_kind kind)
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
    shadercOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
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

Scope<GraphicsShader> OpenGLGraphicsShaderFactory::Create(const Path &path)
{
    auto shader = CreateScope<GraphicsShader>(
        CreateScope<OpenGLShaderBackend>(),
        CreateScope<OpenGLGraphicsShaderBackend>()
    );

    std::string source = FileSystem::ReadText(path);
    auto [vertexSource, fragmentSource, geometrySource] = ParseShader(source);

    auto vertexVulkanSpv = CompileVulkanSpv(vertexSource, path.filename().stem(), shaderc_glsl_vertex_shader);
    auto vertexCode = CompileOpenGLSpv(vertexVulkanSpv, path.filename().stem(), shaderc_glsl_vertex_shader);
    AccessVertexModule(shader) = OpenGLVertexModuleFactory::Create(vertexCode);

    auto fragmentVulkanSpv = CompileVulkanSpv(fragmentSource, path.filename().stem(), shaderc_glsl_fragment_shader);
    auto fragmentCode = CompileOpenGLSpv(fragmentVulkanSpv, path.filename().stem(), shaderc_glsl_fragment_shader);
    AccessFragmentModule(shader) = OpenGLFragmentModuleFactory::Create(fragmentCode);

    CombineModules(shader);

    auto programID = shader->GetShaderBackend().As<OpenGLShaderBackend>()->GetRendererID();
    auto vertexShaderID = shader->GetVertexModule()->GetShaderModuleBackend().As<OpenGLShaderModuleBackend>()->GetRendererID();
    auto fragmentShaderID = shader->GetFragmentModule()->GetShaderModuleBackend().As<OpenGLShaderModuleBackend>()->GetRendererID();

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    glLinkProgram(programID);

    return shader;
}
}
