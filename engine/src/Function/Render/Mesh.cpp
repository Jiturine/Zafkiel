#include "Function/Render/Mesh.h"
#include "Function/RHI/RHICommandList.h"
#include "Resource/AssetManager.h"

namespace Zafkiel
{

Mesh::Mesh(AssetHandle meshAssetHandle)
{
    auto meshAsset = AssetManager::Instance().GetAsset(meshAssetHandle).As<MeshAsset>();
    auto &vertices = meshAsset->GetVertices();
    auto &indices = meshAsset->GetIndices();

    RHIBufferDesc vertexBufferDesc
    {
        .size = (uint32)(vertices.size() * sizeof(MeshVertex)),
        .usages = BufferUsageFlags::VertexBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Static,
    };
    vertexBuffer = GlobalRHICmdList->CreateBuffer(vertexBufferDesc, meshAsset->GetVertices().data());

    RHIBufferDesc indexBufferDesc
    {
        .size = (uint32)(indices.size() * sizeof(uint32)),
        .usages = BufferUsageFlags::IndexBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Static,
    };
    indexBuffer = GlobalRHICmdList->CreateBuffer(indexBufferDesc, meshAsset->GetIndices().data());
}

}