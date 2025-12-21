#include "opengl_context.h"
#include <SDL3/SDL_opengl.h>
#include "opengl_vertex_buffer.h"
#include "opengl_index_buffer.h"
#include "opengl_mesh.h"
#include "opengl_frame_buffer.h"
#include "opengl_graphics_shader.h"
#include "opengl_image.h"
#include "opengl_texture.h"
#include "opengl_graphics_pipeline.h"
#include "opengl_render_pass.h"
#include "opengl_render_resource.h"
#include "opengl_render_resource_template.h"
#include "opengl_global_render_resource.h"
#include "opengl_render_pass_resource.h"
#include "opengl_material.h"
#include "opengl_object_render_resource.h"

namespace Zafkiel
{

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
    }

    SDL_GL_MakeCurrent(window.GetHandle(), handle);
}

OpenGLContext::~OpenGLContext()
{
    SDL_GL_DestroyContext(handle);
}

Scope<VertexBuffer> OpenGLContext::CreateVertexBuffer(const float *vertices, uint32_t size) const
{
    return OpenGLVertexBufferFactory::Create(vertices, size);
}
Scope<IndexBuffer> OpenGLContext::CreateIndexBuffer(const uint32_t *indices, uint32_t count) const
{
    return OpenGLIndexBufferFactory::Create(indices, count);
}
Scope<Mesh> OpenGLContext::CreateMesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices) const 
{
    return OpenGLMeshFactory::Create(vertices, indices);
}
Scope<FrameBuffer> OpenGLContext::CreateFrameBuffer(const FrameBufferSpecification &spec) const
{
    return OpenGLFrameBufferFactory::Create(spec);
}
Scope<GraphicsShader> OpenGLContext::CreateGraphicsShader(const Path &path) const
{
    return OpenGLGraphicsShaderFactory::Create(path);
}
Scope<Texture2D> OpenGLContext::CreateTexture2D(const Texture2DSpecification &spec) const
{
    return OpenGLTexture2DFactory::Create(spec);
}
Scope<Image> OpenGLContext::CreateImage(const ImageSpecification &spec) const
{
    return OpenGLImageFactory::Create(spec);
}
Scope<Texture2D> OpenGLContext::CreateTexture2D(const Texture2DSpecification &spec, Buffer buffer) const
{
    return OpenGLTexture2DFactory::Create(spec, buffer);
}
Scope<GraphicsPipeline> OpenGLContext::CreateGraphicsPipeline(const GraphicsPipelineSpecification &spec) const 
{
    return OpenGLGraphicsPipelineFactory::Create(spec);
}
Scope<RenderPass> OpenGLContext::CreateRenderPass(const RenderPassSpecification &spec) const 
{
    return OpenGLRenderPassFactory::Create(spec);
}
Scope<RenderResource> OpenGLContext::CreateRenderResource(const Observer<RenderResourceTemplate> renderResourceTemplate) const
{
    return OpenGLRenderResourceFactory::Create(renderResourceTemplate);
}

Scope<GlobalRenderResource> OpenGLContext::CreateGlobalRenderResource(const Path &path) const 
{
    return OpenGLGlobalRenderResourceFactory::Create(path);
}
Scope<RenderPassResource> OpenGLContext::CreateRenderPassResource(const Path &path) const
{
    return OpenGLRenderPassResourceFactory::Create(path);
}
Scope<Material> OpenGLContext::CreateMaterial(const MaterialSpecification &spec) const 
{
    return OpenGLMaterialFactory::Create(spec);
}
Scope<ObjectRenderResource> OpenGLContext::CreateObjectRenderResource(const Path &path) const 
{
    return OpenGLObjectRenderResourceFactory::Create(path);
}
Scope<RenderResourceTemplate> OpenGLContext::CreateRenderResourceTemplate(const Observer<RenderResourceSchema> schema) const 
{
    return OpenGLRenderResourceTemplateFactory::Create(schema);
}
void OpenGLContext::Resize(uint32_t width, uint32_t height)
{

}

}
