#pragma once

#include "function/render/mesh.h"
#include "resource/asset.h"
#include <vector>

namespace Zafkiel
{
class MeshAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Mesh; }

    MeshAsset(AssetHandle handle, std::vector<MeshVertex> &&vertices, std::vector<uint32_t> &&indices)
        : Asset(handle), vertices(std::move(vertices)), indices(std::move(indices))
    {
        // Renderer::Submit([self = Ref(this), vertices = std::move(vertices), indices = std::move(indices)]() mutable {
        //     self->mesh = CreateScope<Mesh>(vertices, indices);
        // });
    }

    ~MeshAsset()
    {
        // Renderer::Submit([mesh = std::move(mesh)]() mutable {
        //     mesh = nullptr;
        // });
    }

    const std::vector<MeshVertex> &GetVertices() const { return vertices; }
    const std::vector<uint32_t> &GetIndices() const { return indices; }

  private:
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;
};

}
