#pragma once
#include "function/render/graphics_pipeline.h"
#include <glad/glad.h>

namespace Zafkiel
{

GLenum PrimitiveTopologyToOpenGLType(PrimitiveTopology type);
GLenum CullModeToOpenGLType(CullMode mode);
GLenum FrontFaceToOpenGLType(FrontFace type);
GLenum PolygonModeToOpenGLType(PolygonMode mode);

class OpenGLGraphicsPipelineBackend final : public GraphicsPipelineBackend
{
  public:
    OpenGLGraphicsPipelineBackend(const GraphicsPipelineSpecification &spec);
    GLenum GetPrimitiveTopology() const;
    GLenum GetCullMode() const;
    GLenum GetFrontFace() const;
    GLenum GetPolygonMode() const;
    bool GetDepthTest() const;
  private:
    PrimitiveTopology primitiveTopology;
    Observer<GraphicsShader> shader;
    Observer<RenderPass> renderPass;
    CullMode cullMode;
    FrontFace frontFace = FrontFace::CounterClockWise;
    PolygonMode polygonMode;
    bool depthTest;
};

class OpenGLGraphicsPipelineFactory
{
  public:
    static Scope<GraphicsPipeline> Create(const GraphicsPipelineSpecification &spec);
};

}
