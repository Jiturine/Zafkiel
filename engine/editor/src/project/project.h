#pragma once
#include "platform/filesystem/filesystem.h"
#include "resource/editor_asset_manager.h"

namespace Zafkiel
{

struct ProjectConfig
{
    std::string name;
    Path startScene;
    Path assetDirectory;
};

class Project : public RefCounted
{
  public:
    Project(const ProjectConfig &config, Ref<GraphicsContext> context) : config(config), context(context)
    {
        assetManager = MakeRef<EditorAssetManager>(context);
        assetManager->SetAssetPath(config.assetDirectory);
    }

    Ref<EditorAssetManager> GetAssetManager() const { return assetManager; }

    ProjectConfig &GetConfig() { return config; }

    friend class ProjectSerializer;

  private:
    ProjectConfig config;
    Ref<EditorAssetManager> assetManager;
    Ref<GraphicsContext> context;
};
}