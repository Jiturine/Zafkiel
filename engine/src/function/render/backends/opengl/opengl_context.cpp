#include "function/render/backends/opengl/opengl_context.h"
#include <SDL3/SDL_opengl.h>
#include "function/render/backends/opengl/opengl_vertex_buffer.h"
#include "function/render/backends/opengl/opengl_index_buffer.h"
#include "function/render/backends/opengl/opengl_uniform_buffer.h"
#include "function/render/backends/opengl/opengl_frame_buffer.h"
#include "function/render/backends/opengl/opengl_image.h"
#include "function/render/backends/opengl/opengl_texture.h"
#include "function/render/backends/opengl/opengl_graphics_pipeline.h"
#include "function/render/backends/opengl/opengl_render_pass.h"
#include "function/render/backends/opengl/opengl_shader_material.h"
#include "function/render/backends/opengl/opengl_shader_material_template.h"
#include "function/render/backends/opengl/opengl_object_shader_material.h"
#include "function/render/backends/opengl/opengl_object_shader_material_template.h"

namespace Zafkiel
{

static constexpr uint32_t GlobalMaterialBindingBase = 0;
static constexpr uint32_t PassMaterialBindingBase = 16;
static constexpr uint32_t SurfaceMaterialBindingBase = 32;
static constexpr uint32_t ObjectShaderMaterialBindingBase = 48;

OpenGLContext::OpenGLContext(const Window &window)
    : window(window.GetHandle())
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    handle = SDL_GL_CreateContext(window.GetHandle());
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        Log::Critical("Failed to initialize glad!");
    }

    if (!handle)
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

    SDL_GL_MakeCurrent(window.GetHandle(), handle);
}

OpenGLContext::~OpenGLContext()
{
    SDL_GL_DestroyContext(handle);
}

Scope<VertexBufferBackend> OpenGLContext::CreateVertexBufferBackend(const float *vertices, uint32_t size)
{
    uint32_t rendererID;
    glCreateBuffers(1, &rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    return CreateScope<OpenGLVertexBufferBackend>(rendererID);
}
Scope<IndexBufferBackend> OpenGLContext::CreateIndexBufferBackend(const uint32_t *indices, uint32_t count)
{
    uint32_t rendererID;
    glCreateBuffers(1, &rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    return CreateScope<OpenGLIndexBufferBackend>(rendererID, count);
}
Scope<UniformBufferBackend> OpenGLContext::CreateUniformBufferBackend(uint32_t size) 
{
    uint32_t rendererID;
    glCreateBuffers(1, &rendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return CreateScope<OpenGLUniformBufferBackend>(size, rendererID);
}
Scope<FrameBufferBackend> OpenGLContext::CreateFrameBufferBackend(const FrameBufferSpecification &spec, Borrow<RenderPass> renderPass, const std::vector<Borrow<Image>> &images)
{
    uint32_t rendererID;
    glGenFramebuffers(1, &rendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

    uint32_t colorAttachmentIndex = 0;
    std::vector<GLenum> drawBuffers;

    for (auto image : images)
    {
        if (image->GetFormat() == ImageFormat::DEPTH24STENCIL8)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, image->GetBackend().As<OpenGLImageBackend>()->GetRendererID(), 0);
        }
        else if (image->GetFormat() == ImageFormat::DEPTH32F)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, image->GetBackend().As<OpenGLImageBackend>()->GetRendererID(), 0);;
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_TEXTURE_2D, image->GetBackend().As<OpenGLImageBackend>()->GetRendererID(), 0);
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);
            colorAttachmentIndex++;
        }
    }
    if (!drawBuffers.empty())
    {
        glDrawBuffers(drawBuffers.size(), drawBuffers.data());
    }
    else
    {
        glDrawBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Log::Error("Framebuffer is incomplete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return CreateScope<OpenGLFrameBufferBackend>(rendererID);
}
Scope<ShaderBackend> OpenGLContext::CreateShaderBackend(Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule)
{
    auto programID = glCreateProgram();
    auto vertexShaderID = vertexModule->GetBackend().As<OpenGLShaderModuleBackend>()->GetRendererID();
    auto fragmentShaderID = fragmentModule->GetBackend().As<OpenGLShaderModuleBackend>()->GetRendererID();

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    glLinkProgram(programID);

    // 检查链接状态
    GLint linkStatus = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint logLength = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::vector<char> log(logLength);
            glGetProgramInfoLog(programID, logLength, nullptr, log.data());
            Log::Error("Shader program linking failed: {}", log.data());
        } else {
            Log::Error("Shader program linking failed (no error log available)");
        }
    }

    return CreateScope<OpenGLShaderBackend>(programID);
}

Scope<ShaderModuleBackend> OpenGLContext::CreateShaderModuleBackend(Buffer buffer, ShaderStage stage)
{
    auto shaderType = stage == ShaderStage::Vertex ? GL_VERTEX_SHADER :
                                stage == ShaderStage::Fragment ? GL_FRAGMENT_SHADER :
                                GL_GEOMETRY_SHADER;

    uint32_t rendererID = glCreateShader(shaderType);

    glShaderBinary(1, &rendererID, GL_SHADER_BINARY_FORMAT_SPIR_V,
                   buffer.Data<uint8_t>(), buffer.Size<uint8_t>());

    glSpecializeShader(rendererID, "main", 0, nullptr, nullptr);

    // 检查编译状态
    GLint compileStatus = 0;
    glGetShaderiv(rendererID, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) {
        GLint logLength = 0;
        glGetShaderiv(rendererID, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::vector<char> log(logLength);
            glGetShaderInfoLog(rendererID, logLength, nullptr, log.data());
            Log::Error("SPIR-V shader specialization failed: {}", log.data());
        } else {
            Log::Error("SPIR-V shader specialization failed (no error log available)");
        }
        glDeleteShader(rendererID);
        return nullptr;
    }
    return CreateScope<OpenGLShaderModuleBackend>(rendererID);
}

Scope<ImageBackend> OpenGLContext::CreateImageBackend(const ImageSpecification &spec)
{
    uint32_t rendererID;
	glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    OpenGLImageFormat format = ImageFormatToOpenGLType(spec.format);
    glTextureStorage2D(rendererID, 1, format.internalFormat, spec.width, spec.height);
    return CreateScope<OpenGLImageBackend>(rendererID);
}

Scope<Texture2DBackend> OpenGLContext::CreateTexture2DBackend(const Texture2DSpecification &spec, Borrow<ImageBackend> imageBackend)
{
    auto glImageBackend = imageBackend.As<OpenGLImageBackend>();
    auto imageRendererID = glImageBackend->GetRendererID();
    return CreateScope<OpenGLTexture2DBackend>(spec, imageRendererID);
}

Scope<PipelineBackend> OpenGLContext::CreatePipelineBackend(const GraphicsPipelineSpecification &spec, Borrow<GraphicsShader> shader, Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule, Borrow<RenderPass> renderPass, Borrow<RenderRegistryView> registryView)
{
    return CreateScope<OpenGLPipelineBackend>();
}

Scope<RenderPassBackend> OpenGLContext::CreateRenderPassBackend(const RenderPassSpecification &spec)
{
    return CreateScope<OpenGLRenderPassBackend>();
}

Scope<ShaderMaterialBackend> OpenGLContext::CreateShaderMaterialBackend(Borrow<ShaderMaterialTemplateBackend> _shaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema)
{
    std::vector<std::optional<Scope<UniformBuffer>>> uniformBuffers(schema->GetResourceTypeInfos().size());
    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schema->GetResourceTypeInfos()))
    {
        if (!resourceTypeInfo) continue;
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            uint32_t size = resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>()->GetSize();
            auto uniformBufferBackend = CreateUniformBufferBackend(size);
            uniformBuffers[binding] = CreateScope<UniformBuffer>(size, std::move(uniformBufferBackend));
        }
    }
    return CreateScope<OpenGLShaderMaterialBackend>(std::move(uniformBuffers));
}

constexpr size_t AlignUp(size_t value, size_t alignment) 
{
    return (value + alignment - 1) & ~(alignment - 1);
}
Scope<ObjectShaderMaterialBackend> OpenGLContext::CreateObjectShaderMaterialBackend(Borrow<ObjectShaderMaterialTemplateBackend> objectShaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema)
{
    std::vector<std::optional<Scope<UniformBuffer>>> uniformBuffers(schema->GetResourceTypeInfos().size());
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schema->GetResourceTypeInfos()))
    {
        if (!resourceTypeInfo) continue;
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBufferType = resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>();
            uint32_t perObjectsize = uniformBufferType->GetSize();
            uint32_t stride = AlignUp(perObjectsize, alignment);
            uint32_t bufferSize = stride * maxObjectNum;
            auto uniformBufferBackend = CreateUniformBufferBackend(bufferSize);
            uniformBuffers[binding] = CreateScope<UniformBuffer>(bufferSize, std::move(uniformBufferBackend));
        }
    }
    return CreateScope<OpenGLObjectShaderMaterialBackend>(std::move(uniformBuffers));
}

Scope<ObjectShaderMaterialTemplateBackend> OpenGLContext::CreateObjectShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema)
{
    return CreateScope<OpenGLObjectShaderMaterialTemplateBackend>();
}

Scope<ShaderMaterialTemplateBackend> OpenGLContext::CreateShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema)
{
    return CreateScope<OpenGLShaderMaterialTemplateBackend>();
}

void OpenGLContext::SetImageData(Borrow<Image> image, Buffer buffer)
{
    auto imageBackend = image->GetBackend().As<OpenGLImageBackend>();

    OpenGLImageFormat format = ImageFormatToOpenGLType(image->GetFormat());
    uint32_t bytes = ImageFormatToBytes(image->GetFormat());
    if (buffer.Size<uint8_t>() != image->GetWidth() * image->GetHeight() * bytes)
    {
        Log::Error("Data must be entire texture! Expected: {}, Got: {}", image->GetWidth() * image->GetHeight() * bytes, buffer.Size<uint8_t>());
        return;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(imageBackend->GetRendererID(), 0, 0, 0,
        image->GetWidth(), image->GetHeight(), format.dataFormat, format.dataType, buffer.Data<uint8_t>());
}
void OpenGLContext::Resize(uint32_t width, uint32_t height)
{

}

void OpenGLContext::BindGlobalMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView)
{
    auto backend = material->GetBackend().As<OpenGLShaderMaterialBackend>();
    UploadShaderMaterialUniform(backend, material->GetResources());

    for (auto [binding, resource] : std::views::enumerate(material->GetResources()))
    {
        if (!resource) continue;
        if (resource.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBuffer = Borrow(backend->GetUniformBuffer(binding));
            glBindBufferBase(GL_UNIFORM_BUFFER, GlobalMaterialBindingBase + binding, uniformBuffer->GetBackend().As<OpenGLUniformBufferBackend>()->GetRendererID());
        }
        else if (resource.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            if (!resource.value().handle) continue;
            auto texture2D = registryView->GetTexture2D(resource.value().handle.value());
            auto image = registryView->GetImage(texture2D->GetImage());
            auto imageBackend = image->GetBackend().As<OpenGLImageBackend>();
            glBindTextureUnit(GlobalMaterialBindingBase + binding, imageBackend->GetRendererID());
        }
    }
}
void OpenGLContext::BindPassMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView)
{
    auto backend = material->GetBackend().As<OpenGLShaderMaterialBackend>();
    UploadShaderMaterialUniform(backend, material->GetResources());

    for (auto [binding, resource] : std::views::enumerate(material->GetResources()))
    {
        if (!resource) continue;
        if (resource.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBuffer = Borrow(backend->GetUniformBuffer(binding));
            glBindBufferBase(GL_UNIFORM_BUFFER, PassMaterialBindingBase + binding, uniformBuffer->GetBackend().As<OpenGLUniformBufferBackend>()->GetRendererID());
        }
        else if (resource.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            if (!resource.value().handle) continue;
            auto texture2D = registryView->GetTexture2D(resource.value().handle.value());
            auto image = registryView->GetImage(texture2D->GetImage());
            auto imageBackend = image->GetBackend().As<OpenGLImageBackend>();
            glBindTextureUnit(PassMaterialBindingBase + binding, imageBackend->GetRendererID());
        }
    }
}
void OpenGLContext::BindSurfaceMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView)
{
    auto backend = material->GetBackend().As<OpenGLShaderMaterialBackend>();
    UploadShaderMaterialUniform(backend, material->GetResources());

    for (auto [binding, resource] : std::views::enumerate(material->GetResources()))
    {
        if (!resource) continue;
        if (resource.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBuffer = Borrow(backend->GetUniformBuffer(binding));
            glBindBufferBase(GL_UNIFORM_BUFFER, SurfaceMaterialBindingBase + binding, uniformBuffer->GetBackend().As<OpenGLUniformBufferBackend>()->GetRendererID());
        }
        else if (resource.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            if (!resource.value().handle) continue;
            auto texture2D = registryView->GetTexture2D(resource.value().handle.value());
            auto image = registryView->GetImage(texture2D->GetImage());
            auto imageBackend = image->GetBackend().As<OpenGLImageBackend>();
            glBindTextureUnit(SurfaceMaterialBindingBase + binding, imageBackend->GetRendererID());
        }
    }
}

void OpenGLContext::BindObjectShaderMaterial(uint32_t index, Borrow<ObjectShaderMaterial> material, Borrow<RenderRegistryView> registryView)
{
    auto schema = Borrow(registryView->GetShaderMaterialSchema(material->GetSchema()));
    auto backend = material->GetBackend().As<OpenGLObjectShaderMaterialBackend>();
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schema->GetResourceTypeInfos()))
    {
        if (!resourceTypeInfo) continue;
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBufferType = resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>();
            uint32_t perObjectsize = uniformBufferType->GetSize();
            uint32_t stride = AlignUp(perObjectsize, alignment);
            auto uniformBufferRendererID = backend->GetUniformBuffer(binding)->GetBackend().As<OpenGLUniformBufferBackend>()->GetRendererID();

            glBindBufferRange(GL_UNIFORM_BUFFER, ObjectShaderMaterialBindingBase + binding, uniformBufferRendererID,
            index * stride, perObjectsize);
        }
    }
}

void OpenGLContext::UploadShaderMaterialUniform(Borrow<OpenGLShaderMaterialBackend> shaderMaterialBackend, const std::vector<std::optional<ShaderResource>> &resources)
{
    for (auto [binding, resource] : std::views::enumerate(resources))
    {
        if (!resource) continue;
        if (resource.value().type->GetCategory() != ShaderReflection::ResourceTypeCategory::UniformBlock) continue;

        auto uniformBufferBackend = shaderMaterialBackend->GetUniformBuffer(binding)->GetBackend().As<OpenGLUniformBufferBackend>();
        auto src = resource.value().uniformBuffer.Data<uint8_t>();
        auto size = resource.value().uniformBuffer.Size<uint8_t>();
        uniformBufferBackend->SetData(0, size, src);
    }
}

void OpenGLContext::UploadObjectShaderMaterialUniform(Borrow<ObjectShaderMaterialBackend> objectShaderMaterialBackend, const std::vector<std::optional<ObjectShaderResource>> &resources)
{
    auto backend = objectShaderMaterialBackend.As<OpenGLObjectShaderMaterialBackend>();
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    for (auto [binding, resource] : std::views::enumerate(resources))
    {
        if (!resource) continue;
        if (resource.value().type->GetCategory() != ShaderReflection::ResourceTypeCategory::UniformBlock) continue;

        auto uniformBufferBackend = backend->GetUniformBuffer(binding)->GetBackend().As<OpenGLUniformBufferBackend>();
        auto uniformBufferType = resource.value().type->As<ShaderReflection::UniformBlock>();
        uint32_t perObjectsize = uniformBufferType->GetSize();
        uint32_t stride = AlignUp(perObjectsize, alignment);
        for (uint32_t i = 0; i < maxObjectNum; i++)
        {
            auto src = resource.value().dynamicUniformBuffer.Data<uint8_t>() + i * perObjectsize;
            uniformBufferBackend->SetData(i * stride, perObjectsize, src);
        }
    }
}

void OpenGLContext::DrawIndexed(Borrow<VertexBuffer> vertexBuffer, Borrow<IndexBuffer> indexBuffer) 
{
    auto vertexBufferBackend = vertexBuffer->GetBackend().As<OpenGLVertexBufferBackend>();
    auto indexBufferBackend = indexBuffer->GetBackend().As<OpenGLIndexBufferBackend>();

    uint32_t vertexArray;
    glCreateVertexArrays(1, &vertexArray); // TODO: VAO Cache
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferBackend->GetRendererID());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferBackend->GetRendererID());

    uint32_t index = 0;
    const auto &layout = currentShader->As<GraphicsShader>()->GetReflection().vertexInput;

    for (auto element : layout.elements)
    {
        switch (element.type)
        {
            using enum ShaderFundamentalType;
        case Float: case Float2: case Float3: case Float4:
        case Mat3: case Mat4:
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				ShaderDataTypeCount(element.type),
				ShaderDataTypeToOpenGLBaseType(element.type),
                false,
				layout.stride,
				reinterpret_cast<const void *>(element.offset));
            break;
        case Int: case Int2: case Int3: case Int4:
        case UInt: case UInt2: case UInt3: case UInt4:
        case Bool:
			glEnableVertexAttribArray(index);
			glVertexAttribIPointer(
				index,
				ShaderDataTypeCount(element.type),
				ShaderDataTypeToOpenGLBaseType(element.type),
				layout.stride,
				reinterpret_cast<const void *>(element.offset));
            break;

        default:
            Log::Error("Unsupported Shader Data Type!");
            break;
        }

        index++;
    }
    glDrawElements(PrimitiveTopologyToOpenGLType(currentPipeline.value().As<GraphicsPipeline>()->GetPrimitiveTopology()),
        indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
    
    glDeleteVertexArrays(1, &vertexArray);
}

void OpenGLContext::BindGraphicsPipeline(Borrow<GraphicsPipeline> pipeline, Borrow<RenderRegistryView> registryView) 
{
    if (auto cullFace = CullModeToOpenGLType(pipeline->GetCullMode()))
    {
        glEnable(GL_CULL_FACE);
        glCullFace(cullFace);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    glFrontFace(FrontFaceToOpenGLType(pipeline->GetFrontFace()));

    glPolygonMode(GL_FRONT_AND_BACK, PolygonModeToOpenGLType(pipeline->GetPolygonMode()));

    // 设置深度测试
    if (pipeline->GetDepthTest())
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    auto shader = Borrow(registryView->GetGraphicsShader(pipeline->GetShader()));
    auto shaderBackend = shader->GetBackend().As<OpenGLShaderBackend>();
    glUseProgram(shaderBackend->GetRendererID());

    currentPipeline = pipeline;
    currentShader = shader;
}

static void ClearColor(uint32_t attachmentIndex, ClearValue value)
{
    switch (value.format)
    {
        using enum ImageFormat;
    case R8:
    case R8_sRGB:
        glClearBufferfv(GL_COLOR, attachmentIndex, &value.floatValue);
        break;
    case RG8:
    case RG8_sRGB:
        glClearBufferfv(GL_COLOR, attachmentIndex, (float*)&value.vec2Value);
        break;
    case BGR8:
    case RGB8:
    case RGB8_sRGB:
    case BGR8_sRGB:
    case RGB16F:
        glClearBufferfv(GL_COLOR, attachmentIndex, (float*)&value.vec3Value);
        break;
    case BGRA8:
    case RGBA8:
    case RGBA8_sRGB:
    case BGRA8_sRGB:
    case RGBA16F:
    case RGBA32F:
        glClearBufferfv(GL_COLOR, attachmentIndex, (float*)&value.vec4Value);
        break;
    case R32UI:
        glClearBufferuiv(GL_COLOR, attachmentIndex, &value.uintValue);
        break;
    case R32F:
        glClearBufferfv(GL_COLOR, attachmentIndex, &value.floatValue);
        break;
    default:
        Log::Error("Unsupported Clear Color Value!");
        break;
    }
}

static void ClearDepthStencil(ClearValue value)
{
    glClearBufferfi(GL_DEPTH_STENCIL, 0, value.floatValue, value.uintValue);
}

static void ClearDepth(ClearValue value)
{
    glClearBufferfv(GL_DEPTH, 0, &value.floatValue);
}

static void ClearStencil(ClearValue value)
{
    glClearBufferuiv(GL_STENCIL, 0, &value.uintValue);
}

void OpenGLContext::BeginRenderPass(Borrow<RenderPass> renderPass, Borrow<FrameBuffer> frameBuffer, const std::vector<ClearValue> &clearValues)
{
    auto frameBufferBackend = frameBuffer->GetBackend().As<OpenGLFrameBufferBackend>();

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferBackend->GetRendererID());
    glViewport(0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());

    // 启用颜色写入
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    uint32_t colorAttachmentIndex = 0;
    for (auto &clearValue : clearValues)
    {
        if (clearValue.type == AttachmentType::Color)
        {
            ClearColor(colorAttachmentIndex, clearValue);
            colorAttachmentIndex++;
        }
        else if (clearValue.type == AttachmentType::DepthStencil)
            ClearDepthStencil(clearValue);
        else if (clearValue.type == AttachmentType::Depth)
            ClearDepth(clearValue);
        else if (clearValue.type == AttachmentType::Stencil)
            ClearStencil(clearValue);
    }

    currentFrameBuffer = frameBuffer;
}

void OpenGLContext::EndRenderPass() 
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    currentFrameBuffer = std::nullopt;
}

}
