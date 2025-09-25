#pragma once
#include "platform/filesystem/filesystem.h"
#include "resource/editor_asset_manager.h"

namespace Zafkiel
{

struct [[refl]] ProjectConfig
{
    std::string name;
    Path startScene;
    Path assetDirectory;
    Path libraryDirectory;
};

class Project : public RefCounted
{
  public:
    Project(const ProjectConfig &config) : config(config)
    {
        assetManager = MakeRef<EditorAssetManager>();
        assetManager->SetAssetDirectory(config.assetDirectory);
    }

    Ref<EditorAssetManager> GetAssetManager() const { return assetManager; }

    std::string GetName() const { return config.name; }
    Path GetAssetDirectory() const { return config.assetDirectory; }
    Path GetLibraryDirectory() const { return config.libraryDirectory; }
    Path GetStartSceneDirectory() const { return config.startScene; }

  private:
    ProjectConfig config;
    Ref<EditorAssetManager> assetManager;
};

}