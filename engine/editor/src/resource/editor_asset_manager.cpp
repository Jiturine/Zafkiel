#include "editor_asset_manager.h"
#include "asset_importer.h"

namespace Zafkiel
{
EditorAssetManager::EditorAssetManager(Ref<GraphicsContext> context)
{
    assetImporter = MakeRef<AssetImporter>(context);
}

bool EditorAssetManager::IsAssetValid(AssetHandle handle) const
{
    return assetRegistry.contains(handle);
}
bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
{
    return loadedAssets.contains(handle);
}

Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle) const
{
    if (!IsAssetValid(handle)) return nullptr;

    Ref<Asset> asset;
    if (IsAssetLoaded(handle))
    {
        asset = loadedAssets.at(handle);
    }
    else
    {
        const AssetMetadata &metadata = assetRegistry.at(handle);
        asset = assetImporter->ImportAsset(metadata);
        if (!asset)
        {
            Log::CoreError("Asst import failed!");
        }
    }
    return asset;
}

}