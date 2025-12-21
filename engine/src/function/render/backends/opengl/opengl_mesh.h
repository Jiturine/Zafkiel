#pragma once
#include "function/render/mesh.h"
#include "opengl_vertex_buffer.h"
#include "opengl_index_buffer.h"

namespace Zafkiel 
{

class OpenGLMeshFactory 
{
  public:
    static Scope<Mesh> Create(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices)
    {
        const float *data = reinterpret_cast<const float*>(vertices.data());
        uint32_t size = vertices.size() * sizeof(MeshVertex);
        auto vertexBuffer = OpenGLVertexBufferFactory::Create(data, size);
        auto indexBuffer = OpenGLIndexBufferFactory::Create(indices.data(), indices.size());
        return CreateScope<Mesh>(std::move(vertexBuffer), std::move(indexBuffer));
    }
};

}