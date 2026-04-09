#pragma once
#include "Function/RHI/RHIResources.h"
#include "Resource/MeshAsset.h"

namespace Zafkiel
{

class Mesh final : public RefCounted
{
  public:
    Mesh(AssetHandle meshAssetHandle);
  
    Ref<RHIBuffer> GetVertexBuffer() const { return vertexBuffer; }

    Ref<RHIBuffer> GetIndexBuffer() const { return indexBuffer; }

  private:
    Ref<RHIBuffer> vertexBuffer;
    Ref<RHIBuffer> indexBuffer;
};

}
