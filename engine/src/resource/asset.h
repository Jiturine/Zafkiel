#pragma once

#include "core/base/uuid.h"
#include "core/meta/serializer/serialize.h"

namespace Zafkiel
{

using AssetHandle = UUID;

enum class [[refl]] AssetType
{
    None = 0,
    Texture2D
};

class Asset : public RefCounted
{
  public:
    AssetHandle handle;

    virtual AssetType GetType() const = 0;
};

}