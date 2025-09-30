#pragma once
#include "function/render/mesh.h"
#include "opengl_vertex_array.h"

namespace Zafkiel
{
struct OpenGLMesh : public Mesh
{
    OpenGLMesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices);
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    Ref<OpenGLVertexArray> vertexArray;
    Ref<OpenGLVertexBuffer> vertexBuffer;
    Ref<OpenGLIndexBuffer> indexBuffer;

    virtual Ref<VertexArray> GetVertexArray() const override { return vertexArray; }

    // uint32_t materialIndex = 0;
};

}