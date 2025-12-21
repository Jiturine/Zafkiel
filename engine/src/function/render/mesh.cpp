#include "mesh.h" 
#include "renderer.h"

namespace Zafkiel 
{
Mesh::Mesh(Scope<VertexBuffer> vertexBuffer, Scope<IndexBuffer> indexBuffer)
    : vertexBuffer(std::move(vertexBuffer)), indexBuffer(std::move(indexBuffer))
{
    
}
} 