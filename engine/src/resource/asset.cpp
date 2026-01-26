#include "resource/asset.h"

namespace Zafkiel 
{

AssetMetadata::AssetMetadata(AssetHandle handle, AssetType type) 
    : handle(handle), type(type)
{
    switch (type) 
    {
        using enum AssetType;
    case Texture2D: detail = CreateScope<Texture2DMetadata>(); break;
    case CubeMap: detail = CreateScope<CubeMapMetadata>(); break;
    case Model: detail = CreateScope<ModelMetadata>(); break;
    case Mesh: detail = CreateScope<MeshMetadata>(); break;
    case Material: detail = CreateScope<MaterialMetadata>(); break;
    case Shader: detail = CreateScope<ShaderMetadata>(); break;
    case Scene: detail = CreateScope<SceneMetadata>(); break;
    default:
        Log::Error("Unknown Asset Type!");
        break;
    }
}

AssetMetadata::AssetMetadata(AssetHandle handle, AssetType type, const std::vector<AssetHandle> &dependencies)
    : AssetMetadata(handle, type) 
{
    this->dependencies = dependencies;
}

}