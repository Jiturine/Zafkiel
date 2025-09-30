#pragma once
#include "resource/asset.h"
#include "mesh.h"

namespace Zafkiel
{
class Model : public Asset
{
  public:
    Model(const std::vector<Ref<Mesh>> &meshes) : meshes(meshes) {}
    const std::vector<Ref<Mesh>> &GetMeshes() const { return meshes; }
    virtual AssetType GetType() const override { return AssetType::Model; }
  private:
    std::vector<Ref<Mesh>> meshes;
};

}