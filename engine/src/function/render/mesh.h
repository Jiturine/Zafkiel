#pragma once
#include "function/render/vertex_buffer.h"
#include "function/render/index_buffer.h"

namespace Zafkiel
{

struct MeshVertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
};

class Mesh final
{
  public:
    Mesh(RenderHandle vertexBuffer, RenderHandle indexBuffer)
        : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer) {}
  
    RenderHandle GetVertexBuffer() const { return vertexBuffer; }
    RenderHandle GetIndexBuffer() const { return indexBuffer; }

  private:
    RenderHandle vertexBuffer;
    RenderHandle indexBuffer;
};

}
