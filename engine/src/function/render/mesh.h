#pragma once
#include "resource/asset.h"
#include "vertex_array.h"

namespace Zafkiel
{

struct MeshVertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
};

class Mesh : public Asset
{
  public:
    virtual Ref<VertexArray> GetVertexArray() const = 0;
    virtual AssetType GetType() const override { return AssetType::Mesh; }
};

}