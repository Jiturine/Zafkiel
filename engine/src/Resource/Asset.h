#pragma once

#include "Core/Base/UUID.h"
#include "Core/Meta/Reflection/Refl.h"
#include "Core/Base/ImageFormat.h"

namespace Zafkiel
{

using AssetHandle = UUID;

enum class [[refl]] AssetType
{
    None = 0,
    Texture2D,
    CubeMap,
    Model,
    Mesh,
    Material,
    Shader,
    Scene,
    Font
};

struct AssetMetadataBase
{
  public:
    virtual ~AssetMetadataBase() = default;
};

struct AssetMetadata : public RefCounted
{
    AssetType type;
    AssetHandle handle;
    std::vector<AssetHandle> dependencies;

    AssetMetadata(AssetHandle handle, AssetType type, const std::vector<AssetHandle> &dependencies);
    AssetMetadata(AssetHandle handle, AssetType type);

    Scope<AssetMetadataBase> detail; 
};

struct MeshMetadata final : public AssetMetadataBase
{
};

struct ModelMetadata final : public AssetMetadataBase
{
};

struct Texture2DMetadata final : public AssetMetadataBase
{
    ImageFormat format;
};

struct CubeMapMetadata final : public AssetMetadataBase
{
};

struct ShaderMetadata final : public AssetMetadataBase
{
    std::string name;
};

struct SceneMetadata final : public AssetMetadataBase
{
};

struct MaterialMetadata final : public AssetMetadataBase
{
    std::string name;
};

struct FontMetadata final : public AssetMetadataBase
{
};

class Asset : public RefCounted
{
  public:
    virtual ~Asset() = default;
    
    Asset(AssetHandle handle) : handle(handle) {}
    AssetHandle handle;

    virtual AssetType GetAssetType() const = 0;
};

}