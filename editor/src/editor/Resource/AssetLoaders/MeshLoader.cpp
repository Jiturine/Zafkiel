#include "Core/Meta/Serializer/BinarySerializer.h"
#include "Resource/MeshAsset.h"
#include "editor/Resource/AssetLoaders/MeshLoader.h"
#include "editor/Resource/EditorAssetManager.h"

namespace Zafkiel 
{

Ref<Asset> MeshLoader::Load(const Ref<EditorAssetMetadata> &metadata)
{
    ScopedBuffer data = FileSystem::ReadBytes(EditorAssetManager::Instance().GetAssetDirectory() / metadata->filePath);
    BinaryDeserializer meshData(data.Data<uint8>(), data.Size<uint8>());

    auto &verticesData = meshData["Vertices"];
    auto [vData, vSize] = verticesData.Unpack();
    std::vector<MeshVertex> vertices(reinterpret_cast<const MeshVertex *>(vData), 
        reinterpret_cast<const MeshVertex *>(vData + vSize));
    
    auto &indicesData = meshData["Indices"];
    auto [iData, iSize] = indicesData.Unpack();
    std::vector<uint32> indices(reinterpret_cast<const uint32 *>(iData), 
        reinterpret_cast<const uint32 *>(iData + iSize));

    Ref<MeshAsset> meshAsset = CreateRef<MeshAsset>(metadata->handle, MoveTemp(vertices), MoveTemp(indices));
    return meshAsset;
}

}