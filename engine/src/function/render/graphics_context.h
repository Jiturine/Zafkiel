#pragma once
#include "function/render/frame_buffer.h"
#include "function/render/global_material.h"
#include "function/render/pass_material.h"
#include "function/render/object_shader_material.h"
#include "function/render/index_buffer.h"
#include "function/render/image.h"
#include "function/render/graphics_pipeline.h"
#include "function/render/shader.h"
#include "function/render/vertex_module.h"
#include "function/render/fragment_module.h"
#include "function/render/texture.h"
#include "function/render/surface_material.h"
#include "function/render/mesh.h"
#include "function/render/render_pass.h"
#include "platform/filesystem/filesystem.h"
#include "function/render/graphics_api.h"
#include "function/render/shader_material_template.h"

namespace Zafkiel
{

class GraphicsContext
{
  public:
    virtual ~GraphicsContext() = default;
    virtual void Resize(uint32_t width, uint32_t height) = 0;

    virtual Scope<VertexBufferBackend> CreateVertexBufferBackend(const float *vertices, uint32_t size) = 0;
    virtual Scope<IndexBufferBackend> CreateIndexBufferBackend(const uint32_t *indices, uint32_t count) = 0;
    virtual Scope<UniformBufferBackend> CreateUniformBufferBackend(uint32_t size) = 0;
    virtual Scope<FrameBufferBackend> CreateFrameBufferBackend(const FrameBufferSpecification &spec, Borrow<RenderPass> renderPass, const std::vector<Borrow<Image>> &images) = 0;
    virtual Scope<PipelineBackend> CreatePipelineBackend(const GraphicsPipelineSpecification &spec, Borrow<GraphicsShader> shader, Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule, Borrow<RenderPass> renderPass, Borrow<RenderRegistryView> registryView) = 0;
    virtual Scope<RenderPassBackend> CreateRenderPassBackend(const RenderPassSpecification &spec) = 0;
    virtual Scope<ShaderBackend> CreateShaderBackend(Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule) = 0;
    virtual Scope<ImageBackend> CreateImageBackend(const ImageSpecification &spec) = 0;
    virtual Scope<Texture2DBackend> CreateTexture2DBackend(const Texture2DSpecification &spec, Borrow<ImageBackend> imageBackend) = 0;
    virtual Scope<ShaderMaterialTemplateBackend> CreateShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema) = 0;
    virtual Scope<ShaderMaterialBackend> CreateShaderMaterialBackend(Borrow<ShaderMaterialTemplateBackend> shaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema) = 0;
    virtual Scope<ObjectShaderMaterialTemplateBackend> CreateObjectShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema) = 0;
    virtual Scope<ObjectShaderMaterialBackend> CreateObjectShaderMaterialBackend(Borrow<ObjectShaderMaterialTemplateBackend> objectShaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema) = 0;
    virtual Scope<ShaderModuleBackend> CreateShaderModuleBackend(Buffer code, ShaderStage stage) = 0;
    virtual void BindGlobalMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView) = 0;
    virtual void BindPassMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView) = 0;
    virtual void BindSurfaceMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView) = 0;
    virtual void BindObjectShaderMaterial(uint32_t index, Borrow<ObjectShaderMaterial> material, Borrow<RenderRegistryView> registryView) = 0;
    virtual void DrawIndexed(Borrow<VertexBuffer> vertexBuffer, Borrow<IndexBuffer> indexBuffer) = 0;
    virtual void BindGraphicsPipeline(Borrow<GraphicsPipeline> pipeline, Borrow<RenderRegistryView> registryView) = 0;
    virtual void UploadObjectShaderMaterialUniform(Borrow<ObjectShaderMaterialBackend> objectShaderMaterialBackend, const std::vector<std::optional<ObjectShaderResource>> &resources) = 0;
    virtual void BeginRenderPass(Borrow<RenderPass> renderPass, Borrow<FrameBuffer> frameBuffer, const std::vector<ClearValue> &clearValues) = 0;
    virtual void EndRenderPass() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual GraphicsAPI GetAPI() const = 0;
    virtual void WaitIdle() = 0;

  private:
    virtual void SetImageData(Borrow<Image> image, Buffer buffer) = 0;
    friend class Renderer;
};

}
