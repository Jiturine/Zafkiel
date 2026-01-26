#pragma once
#include "function/render/graphics_pipeline.h"
#include <glad/glad.h>

namespace Zafkiel
{

GLenum PrimitiveTopologyToOpenGLType(PrimitiveTopology type);
GLenum CullModeToOpenGLType(CullMode mode);
GLenum FrontFaceToOpenGLType(FrontFace type);
GLenum PolygonModeToOpenGLType(PolygonMode mode);

}
