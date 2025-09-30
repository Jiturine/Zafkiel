#include "opengl_mesh.h"

namespace Zafkiel
{
OpenGLMesh::OpenGLMesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices)
    : vertices(vertices), indices(indices)
{
    vertexArray = MakeRef<OpenGLVertexArray>();
    vertexBuffer = MakeRef<OpenGLVertexBuffer>((float *)vertices.data(), vertices.size() * sizeof(MeshVertex));
    BufferLayout layout = {
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float3, "a_Normal"},
        {ShaderDataType::Float2, "a_TexCoords"}};
    vertexBuffer->SetLayout(layout);
    vertexArray->AddVertexBuffer(vertexBuffer);

    indexBuffer = MakeRef<OpenGLIndexBuffer>(indices.data(), indices.size());
    vertexArray->SetIndexBuffer(indexBuffer);
}

}