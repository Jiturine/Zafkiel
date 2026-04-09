#include "Function/RHI/Backends/OpenGL/OpenGLRHI.h"
#include "Function/RHI/Backends/OpenGL/OpenGLFrameBuffer.h"
#include "Function/RHI/Backends/OpenGL/OpenGLTexture.h"
#include "Function/RHI/Backends/OpenGL/OpenGLBuffer.h"
#include "Function/RHI/Backends/OpenGL/OpenGLShader.h"
#include "Function/RHI/Backends/OpenGL/OpenGLPipeline.h"
#include "Function/RHI/ShaderCompiler/GlslCompiler.h"
#include "Function/RHI/ShaderCompiler/SpirvReflection.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

namespace Zafkiel
{

static constexpr uint32 GlobalMaterialBindingBase = 0;
static constexpr uint32 PipelineMaterialBindingBase = 16;
static constexpr uint32 SurfaceMaterialBindingBase = 32;
static constexpr uint32 ObjectShaderMaterialBindingBase = 48;

OpenGLRHI::OpenGLRHI(Window &window)
    : window(window.GetHandle())
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glContext = SDL_GL_CreateContext(window.GetHandle());
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        Log::Critical("Failed to initialize glad!");
    }

    if (!glContext)
    {
        Log::Error("Could not create an OpenGL context: {}", SDL_GetError());
    }
    else
    {
        Log::Info("OpenGL Info:");
        Log::Info("  Vendor: {}", (const char *)glGetString(GL_VENDOR));
        Log::Info("  Renderer: {}", (const char *)glGetString(GL_RENDERER));
        Log::Info("  Version: {}", (const char *)glGetString(GL_VERSION));

        // 只启用错误和警告，禁用详细信息
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_FALSE);

        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
            if (severity == GL_DEBUG_SEVERITY_HIGH) {
                Log::Error("OpenGL Debug: {}", message);
            }
        }, nullptr);
    }

    SDL_GL_MakeCurrent(window.GetHandle(), glContext);

    frameBufferManager = CreateScope<OpenGLFrameBufferManager>();

    graphicsContext = CreateScope<OpenGLGraphicsContext>(*this);
}

OpenGLRHI::~OpenGLRHI()
{
    SDL_GL_DestroyContext(glContext);
}


Ref<RHIBuffer> OpenGLRHI::CreateBuffer(RHICommandList &RHICmdList, const RHIBufferDesc &desc, const void *data) 
{
    return CreateRef<OpenGLBuffer>(desc, data);
}

Ref<RHITexture> OpenGLRHI::CreateTexture(RHICommandList &RHICmdList, const RHITextureDesc &desc, Buffer data) 
{
    return CreateRef<OpenGLTexture>(desc, data);
}

Ref<RHIGraphicsPipeline> OpenGLRHI::CreateGraphicsPipeline(const RHIGraphicsPipelineDesc &desc)
{
    return CreateRef<OpenGLGraphicsPipeline>(desc);
}

Ref<RHIVertexShader> OpenGLRHI::CreateVertexShader(const Path &path)
{
    std::string source = FileSystem::ReadText(path);

    GlslCompiler compiler(path.parent_path());

    ScopedBuffer code = compiler.Compile(source, GraphicsAPI::OpenGL, ShaderType::Vertex, path.filename().stem().string());

    auto reflection = compiler.GetReflection();

    auto shaderResourceTable = reflection->GetShaderResourceTable();

    return CreateRef<OpenGLVertexShader>(code, MoveTemp(shaderResourceTable));
}

Ref<RHIFragmentShader> OpenGLRHI::CreateFragmentShader(const Path &path)
{
    std::string source = FileSystem::ReadText(path);

    GlslCompiler compiler(path.parent_path());

    ScopedBuffer code = compiler.Compile(source, GraphicsAPI::OpenGL, ShaderType::Fragment, path.filename().stem().string());

    auto reflection = compiler.GetReflection();

    auto shaderResourceTable = reflection->GetShaderResourceTable();

    return CreateRef<OpenGLFragmentShader>(code, MoveTemp(shaderResourceTable));
}

Ref<DynamicUniformBufferContent> OpenGLRHI::CreateDynamicUniformBufferContent(uint32 maxSize, const ShaderReflection::UniformBlock *uniformBlock) 
{
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    return CreateRef<DynamicUniformBufferContent>(maxSize, uniformBlock, alignment);
}

ImTextureRef OpenGLRHI::RegisterImGuiTexture(RHITexture *texture) 
{
    auto glTexture = static_cast<OpenGLTexture *>(texture);

    return glTexture->GetHandle();
}

void OpenGLRHI::UnregisterImGuiTexture(RHITexture *texture) 
{

}
}
