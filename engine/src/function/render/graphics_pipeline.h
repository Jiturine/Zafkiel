#pragma once
#include "function/render/object_shader_material_template.h"
#include "function/render/shader_material_template.h"
#include "function/render/pipeline.h"
#include "function/render/render_pass.h"
#include "function/render/graphics_shader.h"
#include "function/render/vertex_buffer.h"

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
    RenderHandle shader;
    std::vector<RenderHandle> shaderMaterialTemplates;
    RenderHandle renderPass;
    CullMode cullMode;
    FrontFace frontFace = FrontFace::CounterClockWise;
    PolygonMode polygonMode;
    bool depthTest;
    uint32_t colorAttachmentCount;
};

class GraphicsPipeline final : public Pipeline
{
  public:
    GraphicsPipeline(const GraphicsPipelineSpecification &spec, Scope<PipelineBackend> backend)
        : spec(spec), Pipeline(std::move(backend)) {}

    virtual PipelineType GetPipelineType() const override { return PipelineType::Graphics; }

    CullMode GetCullMode() const { return spec.cullMode; }
    FrontFace GetFrontFace() const { return spec.frontFace; }
    PolygonMode GetPolygonMode() const { return spec.polygonMode; }
    PrimitiveTopology GetPrimitiveTopology() const { return spec.primitiveTopology; }
    bool GetDepthTest() const { return spec.depthTest; }
    RenderHandle GetShader() const { return spec.shader; }

  private:
    GraphicsPipelineSpecification spec;
};
}
