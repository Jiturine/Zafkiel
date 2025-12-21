#pragma once
#include "function/render/material.h"
#include "function/render/mesh.h"
#include "function/render/graphics_context.h"
#include "function/scene/entity.h"
#include "resource/asset_manager.h"

namespace Zafkiel
{
#if 0
class EditorRenderer3D : public RefCounted
{
  public:
    EditorRenderer3D()
    {
        // TextureSpecification spec;
        // spec.width = 1, spec.height = 1, spec.format = ImageFormat::RGBA8;
        // auto whiteTextureData = 0xffffffff;
        // Buffer buffer(&whiteTextureData, sizeof(whiteTextureData));
        // whiteTexture = Renderer::Instance().GetGraphicsContext()->CreateTexture2D(spec, buffer);
        // flatShader = Renderer::Instance().GetGraphicsContext()->CreateShader("assets/shaders/outline_shader.glsl");
    }
    void BeginScene(const mat4 &viewProjectionMatrix)
    {
        this->viewProjectionMatrix = viewProjectionMatrix;
    }
    void EndScene()
    {
        this->viewProjectionMatrix = mat4(1.0f);
    }
    void DrawMesh(Ref<Mesh> mesh, Ref<Material> material, const mat4 &modelMatrix, vec3 viewPos, EntityID entityID)
    {
        // auto shader = material->GetShader();
        // shader->Bind();
        // shader->Set("u_ViewProjection", viewProjectionMatrix);
        // shader->Set("u_Model", modelMatrix);
        // shader->Set("u_EntityID", (uint32_t)entityID);
        // shader->Set("u_DiffuseTex", 0);
        // shader->Set("u_ViewPos", viewPos);

        // if (material->HasTexture("diffuseTexture"))
        // {
        //     Scope<Texture2D> diffuseTexture = Engine::GetAssetManager()->GetAsset(material->GetTexture("diffuseTexture")).As<Texture2D>();
        //     uint32_t texID = diffuseTexture->GetRendererID();
        //     diffuseTexture->Bind(0);
        // }
        // else
        // {
        //     whiteTexture->Bind(0);
        // }

        // Ref<VertexArray> vertexArray = mesh->GetVertexArray();
        // Renderer::Instance().GetGraphicsContext()->DrawIndexed(vertexArray, shader);
    }
    void DrawOutline(Ref<Mesh> mesh, const mat4 &modelMatrix)
    {
        // flatShader->Bind();
        // flatShader->Set("u_ViewProjection", viewProjectionMatrix);
        // flatShader->Set("u_Model", modelMatrix);
        // Ref<VertexArray> vertexArray = mesh->GetVertexArray();
        // auto count = vertexArray->GetIndexBuffer()->GetCount();
        // Renderer::Instance().GetGraphicsContext()->DrawIndexed(vertexArray, flatShader);
    }
    Scope<Texture2D> whiteTexture;
  private:
    mat4 viewProjectionMatrix;
    Ref<Shader> flatShader;
};
#endif
}