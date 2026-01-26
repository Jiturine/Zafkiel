#include "editor/resource/asset_loaders/mesh_loader.h"
#include "resource/mesh_asset.h"
#include "core/meta/serializer/binary_serializer.h"
#include "editor/resource/editor_asset_manager.h"

namespace Zafkiel 
{

Ref<Asset> MeshLoader::Load(const Ref<EditorAssetMetadata> &metadata)
{
    ScopedBuffer data = FileSystem::ReadBytes(EditorAssetManager::GetAssetDirectory() / metadata->filePath);
    BinaryDeserializer meshData(data.Data<uint8_t>(), data.Size<uint8_t>());

    auto &verticesData = meshData["Vertices"];
    auto [vData, vSize] = verticesData.Unpack();
    std::vector<MeshVertex> vertices(reinterpret_cast<const MeshVertex *>(vData), 
        reinterpret_cast<const MeshVertex *>(vData + vSize));
    
    auto &indicesData = meshData["Indices"];
    auto [iData, iSize] = indicesData.Unpack();
    std::vector<uint32_t> indices(reinterpret_cast<const uint32_t *>(iData), 
        reinterpret_cast<const uint32_t *>(iData + iSize));

    Ref<MeshAsset> meshAsset = CreateRef<MeshAsset>(metadata->handle, std::move(vertices), std::move(indices));
    return meshAsset;
}

}