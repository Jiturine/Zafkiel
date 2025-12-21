#include "opengl_graphics_pipeline.h"
#include "opengl_pipeline.h"

namespace Zafkiel
{

GLenum PrimitiveTopologyToOpenGLType(PrimitiveTopology type)
{
    switch (type)
    {
        using enum PrimitiveTopology;
    case Triangles: return GL_TRIANGLES;
    case TriangleStrip: return GL_TRIANGLE_STRIP;
    case TriangleFan: return GL_TRIANGLE_FAN;
    default:
        Log::Error("Unknown Primitive Topology!");
        return GL_NONE;
    }
}
GLenum CullModeToOpenGLType(CullMode mode)
{
    switch (mode)
    {
        using enum CullMode;
    case Front: return GL_FRONT;
    case Back: return GL_BACK;
    // TODO: FrontAndBack
    default:
        return GL_NONE;
    }
}
GLenum FrontFaceToOpenGLType(FrontFace type)
{
    switch (type)
    {
        using enum FrontFace;
    case CounterClockWise: return GL_CCW;
    case ClockWise: return GL_CW;
    default:
        return GL_CCW;
    }
}
GLenum PolygonModeToOpenGLType(PolygonMode mode)
{
    switch (mode)
    {
        using enum PolygonMode;
    case Fill: return GL_FILL;
    case Wireframe: return GL_LINE;
    default:
        Log::Error("Unsupported Polygon Mode!");
        return GL_FILL;
    }
}

OpenGLGraphicsPipelineBackend::OpenGLGraphicsPipelineBackend(const GraphicsPipelineSpecification &spec)
    : primitiveTopology(spec.primitiveTopology), shader(spec.shader),
      renderPass(spec.renderPass), cullMode(spec.cullMode), frontFace(spec.frontFace),
      polygonMode(spec.polygonMode), depthTest(spec.depthTest) {}

GLenum OpenGLGraphicsPipelineBackend::GetPrimitiveTopology() const
{
    return PrimitiveTopologyToOpenGLType(primitiveTopology);
}
GLenum OpenGLGraphicsPipelineBackend::GetCullMode() const
{
    return CullModeToOpenGLType(cullMode);
}
GLenum OpenGLGraphicsPipelineBackend::GetFrontFace() const
{
    return FrontFaceToOpenGLType(frontFace);
}
GLenum OpenGLGraphicsPipelineBackend::GetPolygonMode() const
{
    return PolygonModeToOpenGLType(polygonMode);
}
bool OpenGLGraphicsPipelineBackend::GetDepthTest() const
{
    return depthTest;
}

Scope<GraphicsPipeline> OpenGLGraphicsPipelineFactory::Create(const GraphicsPipelineSpecification &spec)
{
    auto pipelineBackend = CreateScope<OpenGLPipelineBackend>();
    auto graphicsPipelineBackend = CreateScope<OpenGLGraphicsPipelineBackend>(spec);
    return CreateScope<GraphicsPipeline>(spec, std::move(pipelineBackend), std::move(graphicsPipelineBackend));
}

}
