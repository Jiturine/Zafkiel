#pragma once
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "function/render/graphics_context.h"
#include "function/render/frame_buffer.h"
#include "function/window/window.h"

namespace Zafkiel
{

class OpenGLContext final : public GraphicsContext
{
  public:
    OpenGLContext(const Window &window);
    ~OpenGLContext();

    virtual Scope<VertexBuffer> CreateVertexBuffer(const float *vertices, uint32_t size) const override;
    virtual Scope<IndexBuffer> CreateIndexBuffer(const uint32_t *indices, uint32_t count) const override;
    virtual Scope<Mesh> CreateMesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices) const override;
    virtual Scope<FrameBuffer> CreateFrameBuffer(const FrameBufferSpecification &spec) const override;
    virtual Scope<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineSpecification &spec) const override;
    virtual Scope<RenderPass> CreateRenderPass(const RenderPassSpecification &spec) const override;
    virtual Scope<Material> CreateMaterial(const MaterialSpecification &spec) const override;
    virtual Scope<GraphicsShader> CreateGraphicsShader(const Path &path) const override;
    virtual Scope<Image> CreateImage(const ImageSpecification &spec) const override;
    virtual Scope<Texture2D> CreateTexture2D(const Texture2DSpecification &spec) const override;
    virtual Scope<Texture2D> CreateTexture2D(const Texture2DSpecification &spec, Buffer buffer) const override;
    virtual Scope<RenderResourceTemplate> CreateRenderResourceTemplate(const Observer<RenderResourceSchema> schema) const override;
    virtual Scope<RenderResource> CreateRenderResource(const Observer<RenderResourceTemplate> renderResourceTemplate) const override;
    virtual Scope<GlobalRenderResource> CreateGlobalRenderResource(const Path &path) const override;
    virtual Scope<RenderPassResource> CreateRenderPassResource(const Path &path) const override;
    virtual Scope<ObjectRenderResource> CreateObjectRenderResource(const Path &path) const override;
    virtual Scope<CubeMap> CreateCubeMap(const std::vector<Path> &paths) const override  { return nullptr; }
    virtual void Resize(uint32_t width, uint32_t height) override;

    virtual void BeginFrame() override {}
    virtual void EndFrame() override { SDL_GL_SwapWindow(window); }
    virtual GraphicsAPI GetAPI() const override { return GraphicsAPI::OpenGL; }

    const SDL_GLContext &GetHandle() const { return handle; }

  private:
    SDL_Window *window;
    SDL_GLContext handle;
};

}