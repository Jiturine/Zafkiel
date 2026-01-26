#pragma once
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "function/render/graphics_context.h"
#include "function/render/frame_buffer.h"
#include "function/render/image.h"
#include "function/render/vertex_module.h"
#include "function/render/fragment_module.h"
#include "function/window/window.h"
#include "function/render/render_registry.h"

#include "function/render/backends/opengl/opengl_vertex_buffer.h"
#include "function/render/backends/opengl/opengl_index_buffer.h"
#include "function/render/backends/opengl/opengl_frame_buffer.h"
#include "function/render/backends/opengl/opengl_image.h"
#include "function/render/backends/opengl/opengl_pipeline.h"
#include "function/render/backends/opengl/opengl_shader.h"
#include "function/render/backends/opengl/opengl_shader_module.h"
#include "function/render/backends/opengl/opengl_texture.h"
#include "function/render/backends/opengl/opengl_shader_material.h"
#include "function/render/backends/opengl/opengl_shader_material_template.h"
#include "function/render/backends/opengl/opengl_graphics_pipeline.h"
#include "function/render/backends/opengl/opengl_render_pass.h"

namespace Zafkiel
{

class OpenGLContext final : public GraphicsContext
{
  public:
    OpenGLContext(const Window &window);
    ~OpenGLContext();

    virtual Scope<VertexBufferBackend> CreateVertexBufferBackend(const float *vertices, uint32_t size) override;
    virtual Scope<IndexBufferBackend> CreateIndexBufferBackend(const uint32_t *indices, uint32_t count) override;
    virtual Scope<UniformBufferBackend> CreateUniformBufferBackend(uint32_t size) override;
    virtual Scope<FrameBufferBackend> CreateFrameBufferBackend(const FrameBufferSpecification &spec, Borrow<RenderPass> renderPass, const std::vector<Borrow<Image>> &images) override;
    virtual Scope<PipelineBackend> CreatePipelineBackend(const GraphicsPipelineSpecification &spec, Borrow<GraphicsShader> shader, Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule, Borrow<RenderPass> renderPass, Borrow<RenderRegistryView> registryView) override;
    virtual Scope<RenderPassBackend> CreateRenderPassBackend(const RenderPassSpecification &spec) override;
    virtual Scope<ShaderBackend> CreateShaderBackend(Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule) override;
    virtual Scope<ImageBackend> CreateImageBackend(const ImageSpecification &spec) override;
    virtual Scope<Texture2DBackend> CreateTexture2DBackend(const Texture2DSpecification &spec, Borrow<ImageBackend> imageBackend) override;
    virtual Scope<ShaderMaterialTemplateBackend> CreateShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema) override;
    virtual Scope<ShaderMaterialBackend> CreateShaderMaterialBackend(Borrow<ShaderMaterialTemplateBackend> shaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema) override;
    virtual Scope<ShaderModuleBackend> CreateShaderModuleBackend(Buffer code, ShaderStage stage) override;
    virtual Scope<ObjectShaderMaterialTemplateBackend> CreateObjectShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema) override;
    virtual Scope<ObjectShaderMaterialBackend> CreateObjectShaderMaterialBackend(Borrow<ObjectShaderMaterialTemplateBackend> objectShaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema) override;
    virtual void SetImageData(Borrow<Image> image, Buffer buffer) override;
    virtual void Resize(uint32_t width, uint32_t height) override;

    virtual void BindGlobalMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView) override;
    virtual void BindPassMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView) override;
    virtual void BindSurfaceMaterial(Borrow<ShaderMaterial> materia, Borrow<RenderRegistryView> registryView) override;
    virtual void BindObjectShaderMaterial(uint32_t index, Borrow<ObjectShaderMaterial> material, Borrow<RenderRegistryView> registryView) override;
    virtual void DrawIndexed(Borrow<VertexBuffer> vertexBuffer, Borrow<IndexBuffer> indexBuffer) override;
    virtual void BindGraphicsPipeline(Borrow<GraphicsPipeline> pipeline, Borrow<RenderRegistryView> registryView) override;
    virtual void BeginRenderPass(Borrow<RenderPass> renderPass, Borrow<FrameBuffer> frameBuffer, const std::vector<ClearValue> &clearValues) override;
    virtual void EndRenderPass() override;
  
    virtual void BeginFrame() override {}
    virtual void EndFrame() override { SDL_GL_SwapWindow(window); }
    virtual GraphicsAPI GetAPI() const override { return GraphicsAPI::OpenGL; }
  
    virtual void UploadObjectShaderMaterialUniform(Borrow<ObjectShaderMaterialBackend> objectShaderMaterialBackend, const std::vector<std::optional<ObjectShaderResource>> &resources) override;
    void UploadShaderMaterialUniform(Borrow<OpenGLShaderMaterialBackend> shaderMaterialBackend, const std::vector<std::optional<ShaderResource>> &resources);
  
    virtual void WaitIdle() override {}

    const SDL_GLContext &GetHandle() const { return handle; }

  private:
    SDL_Window *window;
    SDL_GLContext handle;

    std::optional<Borrow<FrameBuffer>> currentFrameBuffer;
    std::optional<Borrow<Pipeline>> currentPipeline;
    std::optional<Borrow<Shader>> currentShader;
};

}