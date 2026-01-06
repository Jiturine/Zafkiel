#pragma once
#include "resource/asset.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

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
    Mesh(Scope<VertexBuffer> vertexBuffer, Scope<IndexBuffer> indexBuffer);
    const Observer<VertexBuffer> GetVertexBuffer() const { return vertexBuffer; }
    const Observer<IndexBuffer> GetIndexBuffer() const { return indexBuffer; }

  private:
    Scope<VertexBuffer> vertexBuffer;
    Scope<IndexBuffer> indexBuffer;
};

}
