#include "function/render/backends/opengl/opengl_graphics_pipeline.h"

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

}
