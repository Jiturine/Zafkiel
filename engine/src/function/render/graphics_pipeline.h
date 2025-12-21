#pragma once
#include "object_render_resource_template.h"
#include "render_resource_template.h"
#include "pipeline.h"
#include "render_pass.h"
#include "graphics_shader.h"
#include "vertex_buffer.h"

namespace Zafkiel
{

enum class PrimitiveTopology
{
    None = 0,
    Points,
    Lines,
    Triangles,
    LineStrip,
    TriangleStrip,
    TriangleFan
};

enum class CullMode
{
    None = 0,
    Front,
    Back,
    FrontAndBack
};

enum class FrontFace
{
    CounterClockWise,
    ClockWise
};

enum class PolygonMode
{
    Fill,
    Wireframe
};

struct GraphicsPipelineSpecification
{
    PrimitiveTopology primitiveTopology;
    Observer<GraphicsShader> shader;
    std::vector<std::variant<Observer<RenderResourceTemplate>, Observer<ObjectRenderResourceTemplate>>> renderResourceTemplates;
    Observer<RenderPass> renderPass;
    CullMode cullMode;
    FrontFace frontFace = FrontFace::CounterClockWise;
    PolygonMode polygonMode;
    bool depthTest;
    uint32_t colorAttachmentCount;
};

class GraphicsPipelineBackend
{
  public:
    virtual ~GraphicsPipelineBackend() = default;
};

class GraphicsPipeline final : public Pipeline
{
  public:
    GraphicsPipeline(const GraphicsPipelineSpecification &spec, Scope<PipelineBackend> pipelineBackend, Scope<GraphicsPipelineBackend> graphicsShaderBackend)
        : Pipeline(std::move(pipelineBackend)), spec(spec), backend(std::move(graphicsShaderBackend)) {}

    virtual PipelineType GetPipelineType() const override { return PipelineType::Graphics; }

    Observer<GraphicsPipelineBackend> GetGraphicsPipelineBackend() { return backend; }
    const Observer<GraphicsPipelineBackend> GetGraphicsPipelineBackend() const { return backend; }

    Observer<GraphicsShader> GetShader() { return spec.shader; }
    const Observer<GraphicsShader> GetShader() const { return spec.shader; }

  private:
    Scope<GraphicsPipelineBackend> backend;
    GraphicsPipelineSpecification spec;
};
}
