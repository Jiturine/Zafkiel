#include "Function/RHI/ShaderCompiler/GlslCompiler.h"
#include "Function/RHI/ShaderCompiler/GlslIncluder.h"
#include "Function/RHI/ShaderCompiler/SpirvReflection.h"
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <peglib.h>

namespace Zafkiel
{

static constexpr uint32 SetStride = 16;

ScopedBuffer GlslCompiler::Compile(const std::string &source, GraphicsAPI api, ShaderType type, const std::string &name)
{
    shaderc_shader_kind kind = type == ShaderType::Vertex ? shaderc_glsl_vertex_shader
                             : type == ShaderType::Fragment ? shaderc_glsl_fragment_shader
                             : type == ShaderType::Geometry ? shaderc_glsl_geometry_shader
                             : shaderc_glsl_vertex_shader;
    std::string preprocessedSource = Preprocess(source, type);
    ScopedBuffer spv;
    if (api == GraphicsAPI::OpenGL)
    {
        auto vulkanSpv = CompileVulkanSpv(preprocessedSource, name, kind);
        spv = CompileOpenGLSpv(vulkanSpv, name, kind);
    }
    else if (api == GraphicsAPI::Vulkan)
    {
        spv = CompileVulkanSpv(preprocessedSource, name, kind);
    }
    ShaderStage::Stage shaderStage = type == ShaderType::Vertex ? ShaderStage::Vertex :
                                     type == ShaderType::Fragment ? ShaderStage::Fragment : ShaderStage::Geometry;
    spvReflection = CreateScope<ShaderReflection::SpirvReflection>(spv, staticUniformBuffers, api, shaderStage);
    
    return spv;
}

std::string GlslCompiler::Preprocess(const std::string &sourceCode, ShaderType shaderType)
{
    peg::parser parser (R"(

        Start <- (UniformBlockDecl / UniformDecl / .)*

        Qualifier <- < 'static' >?

        UniformDecl <- 'uniform' __ BuiltinType __ InstanceName _ ';'

        UniformBlockDecl <- Qualifier __ < 'uniform' > __ BlockName _ Block _ < InstanceName > _ ';'

        Block <- '{' ( (!'}' .) / Block )* '}'

        BlockName    <- < [a-zA-Z_][a-zA-Z0-9_]* >

        InstanceName <- < [a-zA-Z_][a-zA-Z0-9_]* >
                        
        BuiltinType  <- 'sampler2D' / 'samplerCube'

        _  <- [ \t\r\n]*
        __ <- [ \t\r\n]+
    )");

    struct ModifyOp 
    {
        enum class OpType 
        {
            Insert, Erase
        };
        OpType type;
        size_t pos;
        std::string text;
        size_t eraseSize;
    };

    std::vector<ModifyOp> ops;
    
    uint32 set = shaderType == ShaderType::Vertex ? 0 :
                 shaderType == ShaderType::Fragment ? 1 :
                 shaderType == ShaderType::Geometry ? 2 : 0;

    uint32 binding = 0;

    parser["InstanceName"] = [](const peg::SemanticValues& sv) {
        return sv.token();
    };

    parser["Qualifier"] = [&](const peg::SemanticValues& sv) {
        if (sv.token() == "static")
        {
            ops.push_back({
                .type = ModifyOp::OpType::Erase,
                .pos = (size_t)(sv.token().data() - sv.ss),
                .eraseSize = 7,
            });
            return true;
        }
        return false;
    };

    parser["UniformDecl"] = [&](const peg::SemanticValues& sv) {
        size_t pos = sv.token(0).data() - sv.ss;

        ops.push_back({
            .type = ModifyOp::OpType::Insert,
            .pos = pos,
            .text = std::format("layout(set={}, binding={}) ", set, binding++)
        });
    };
    parser["UniformBlockDecl"] = [&](const peg::SemanticValues& sv) {
        size_t pos = sv.token().data() - sv.ss;

        bool isStatic = std::any_cast<bool>(sv[0]);

        if (isStatic) 
        {
            auto bufferName = sv.token(1);
            staticUniformBuffers.push_back(std::string(bufferName));
        }

        ops.push_back({
            .type = ModifyOp::OpType::Insert,
            .pos = pos,
            .text = std::format("layout(std140, set={}, binding={}) ", set, binding++),
        });
    };;

    bool success = parser.parse(sourceCode);
    if (!success)
    {
        Log::Error("Failed to parse glsl shader!");
        return {};
    }

    std::string result = sourceCode;

    std::sort(ops.begin(), ops.end(),
              [](const ModifyOp& a, const ModifyOp& b) {
                  return a.pos > b.pos;
              });

    for (auto& op : ops) 
    {
        if (op.type == ModifyOp::OpType::Insert)
        {
            result.insert(op.pos, op.text);
        }
        else 
        {
            result.erase(op.pos, op.eraseSize);
        }
    }

    return result;
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

    auto resources = glslCompiler.get_shader_resources();

    for (auto& ubo : resources.uniform_buffers) 
    {
        uint32_t set = glslCompiler.get_decoration(ubo.id, spv::DecorationDescriptorSet);
        uint32_t binding = glslCompiler.get_decoration(ubo.id, spv::DecorationBinding);

        uint32_t gl_binding = set * SetStride + binding;
        glslCompiler.set_decoration(ubo.id, spv::DecorationBinding, gl_binding);
    }

    for (auto& sampled_image : resources.sampled_images) 
    {
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
