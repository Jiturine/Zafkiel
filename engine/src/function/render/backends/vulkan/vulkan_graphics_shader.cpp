#include "vulkan_graphics_shader.h"
#include <shaderc/shaderc.hpp>

namespace Zafkiel
{

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

static ScopedBuffer CompileShader(const std::string &shaderCode, const std::string &shaderName, shaderc_shader_kind kind)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // 设置编译选项
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    // 设置目标环境
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    // 设置源语言
    options.SetSourceLanguage(shaderc_source_language_glsl);
    // 保留名字信息
    options.SetGenerateDebugInfo();
    // 编译着色器
    const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
        shaderCode, // 着色器源代码
        kind,   // 着色器类型
        shaderName.c_str(), // 着色器名称
        options // 编译选项
    );
    // 检查编译结果
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        Log::Error("Failed to Compile Vulkan Shader! Error: {}", result.GetErrorMessage());
    }
    const uint8_t *data = (const uint8_t *)result.cbegin();
    uint32_t size = (result.cend() - result.cbegin()) * sizeof(uint32_t);

    return ScopedBuffer(data, size);
}


Scope<GraphicsShader> VulkanGraphicsShaderFactory::Create(const Path &path, const Scope<VulkanDevice> &device, const Scope<VulkanDescriptorManager> &descriptorManager)
{
    auto shader = CreateScope<GraphicsShader>(
        CreateScope<VulkanShaderBackend>(device, descriptorManager),
        CreateScope<VulkanGraphicsShaderBackend>()
    );
    
    std::string source = FileSystem::ReadText(path);
    auto [vertexSource, fragmentSource, geometrySource] = ParseShader(source);

    auto vertexCode = CompileShader(vertexSource, path.filename().stem(), shaderc_glsl_vertex_shader);
    AccessVertexModule(shader) = VulkanVertexModuleFactory::Create(vertexCode, device);

    auto fragmentCode = CompileShader(fragmentSource, path.filename().stem(), shaderc_glsl_fragment_shader);
    AccessFragmentModule(shader) = VulkanFragmentModuleFactory::Create(fragmentCode, device);

    CombineModules(shader);

    // auto &reflection = shader->GetReflection();
    
    // std::vector<vk::DescriptorSetLayoutBinding> bindings;
    // for (auto &uniformBufferLayout : reflection.uniformBuffers)
    // {
    //     vk::DescriptorSetLayoutBinding binding;
    //     binding.setBinding(uniformBufferLayout.binding)
    //            .setDescriptorCount(1)
    //            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
    //            .setStageFlags(vk::ShaderStageFlagBits::eAll);
    //     bindings.push_back(binding);
    // }
    // for (auto &sampledImageLayout : reflection.sampledImages)
    // {
    //     vk::DescriptorSetLayoutBinding binding;
    //     binding.setBinding(sampledImageLayout.binding)
    //            .setDescriptorCount(1)
    //            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
    //            .setStageFlags(vk::ShaderStageFlagBits::eAll);
    //     bindings.push_back(binding);
    // }

    // vk::DescriptorSetLayoutCreateInfo setLayoutCreateInfo;
    // setLayoutCreateInfo.setBindings(bindings);

    // auto layout = device->GetHandle().createDescriptorSetLayout(setLayoutCreateInfo);
    // shader->GetShaderBackend().As<VulkanShaderBackend>()->GetDescriptorSetLayouts().push_back(std::move(layout));
    
    return shader;
}
}

