#pragma once
#include "Platform/Filesystem/Filesystem.h"
#include "editor/Resource/EditorAssetManager.h"

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
    }

    std::string GetName() const { return config.name; }
    Path GetAssetDirectory() const { return config.assetDirectory; }
    Path GetLibraryDirectory() const { return config.libraryDirectory; }
    Path GetStartSceneDirectory() const { return config.startScene; }

  private:
    ProjectConfig config;
};

}