#pragma once
#include "function/render/mesh.h"
#include "engine.h"
#include "function/render/graphics_context.h"
#include "function/scene/entity.h"

namespace Zafkiel
{

class EditorRenderer3D : public RefCounted
{
  public:
    EditorRenderer3D()
    {
        meshShader = Engine::GetGraphicsContext()->CreateShader("assets/shaders/mesh_shader.glsl");
    }
    void BeginScene(const mat4 &viewProjectionMatrix)
    {
        meshShader->Bind();
        meshShader->Set("u_ViewProjection", viewProjectionMatrix);
    }
    void EndScene()
    {
    }
    void DrawMesh(Ref<Mesh> mesh, const mat4 &modelMatrix, EntityID entityID)
    {
        meshShader->Set("u_Model", modelMatrix);
        meshShader->Set("u_EntityID", (uint32_t)entityID);
        Ref<VertexArray> vertexArray = mesh->GetVertexArray();
        auto count = vertexArray->GetIndexBuffer()->GetCount();
        Engine::GetGraphicsContext()->DrawIndexed(vertexArray, meshShader, vertexArray->GetIndexBuffer()->GetCount());
    }
  private:
    Ref<Shader> meshShader;
};

}