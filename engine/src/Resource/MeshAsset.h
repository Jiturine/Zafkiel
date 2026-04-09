#pragma once

#include "Resource/Asset.h"

namespace Zafkiel
{

struct MeshVertex
{
    vec3 position;
    vec3 normal;
    vec4 tangent;
    vec2 texCoords;
};

class MeshAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Mesh; }

    MeshAsset(AssetHandle handle, std::vector<MeshVertex> &&vertices, std::vector<uint32> &&indices)
        : Asset(handle), vertices(MoveTemp(vertices)), indices(MoveTemp(indices))
    {
    }

    ~MeshAsset()
    {
    }

    const std::vector<MeshVertex> &GetVertices() const { return vertices; }
    const std::vector<uint32> &GetIndices() const { return indices; }

  private:
    std::vector<MeshVertex> vertices;
    std::vector<uint32> indices;
};

}
