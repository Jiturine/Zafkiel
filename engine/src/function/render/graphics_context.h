#pragma once
#include "frame_buffer.h"
#include "global_render_resource.h"
#include "render_pass_resource.h"
#include "object_render_resource.h"
#include "index_buffer.h"
#include "image.h"
#include "graphics_pipeline.h"
#include "shader.h"
#include "texture.h"
#include "material.h"
#include "mesh.h"
#include "platform/filesystem/filesystem.h"
#include "graphics_api.h"
#include "render_resource_template.h"

namespace Zafkiel
{

class GraphicsContext
{
  public:
    virtual ~GraphicsContext() = default;
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    virtual Scope<VertexBuffer> CreateVertexBuffer(const float *vertices, uint32_t size) const = 0;
    virtual Scope<IndexBuffer> CreateIndexBuffer(const uint32_t *indices, uint32_t count) const = 0;
    virtual Scope<Mesh> CreateMesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices) const = 0;
    virtual Scope<FrameBuffer> CreateFrameBuffer(const FrameBufferSpecification &spec) const = 0;
    virtual Scope<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineSpecification &spec) const = 0;
    virtual Scope<RenderPass> CreateRenderPass(const RenderPassSpecification &spec) const = 0;
    virtual Scope<GraphicsShader> CreateGraphicsShader(const Path &path) const = 0;
    virtual Scope<Material> CreateMaterial(const MaterialSpecification &spec) const = 0;
    virtual Scope<Image> CreateImage(const ImageSpecification &spec) const = 0;
    virtual Scope<Texture2D> CreateTexture2D(const Texture2DSpecification &spec) const = 0;
    virtual Scope<Texture2D> CreateTexture2D(const Texture2DSpecification &spec, Buffer buffer) const = 0;
    virtual Scope<RenderResourceTemplate> CreateRenderResourceTemplate(const Observer<RenderResourceSchema> schema) const = 0;
    virtual Scope<RenderResource> CreateRenderResource(const Observer<RenderResourceTemplate> renderResourceTemplate) const = 0;
    virtual Scope<GlobalRenderResource> CreateGlobalRenderResource(const Path &path) const = 0;
    virtual Scope<RenderPassResource> CreateRenderPassResource(const Path &path) const = 0;
    virtual Scope<ObjectRenderResource> CreateObjectRenderResource(const Path &path) const = 0;
    virtual Scope<CubeMap> CreateCubeMap(const std::vector<Path> &paths) const = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual GraphicsAPI GetAPI() const = 0;
};

}