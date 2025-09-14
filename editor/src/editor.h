#pragma once

#include "platform/filesystem/filesystem.h"
#include "project/project.h"
#include "core/meta/reflection/refl.h"

namespace Zafkiel
{

struct [[refl]] EditorConfig
{
    Path startProjectPath;
};

class EditorWindow;

class Editor
{
  public:
    static Ref<EditorWindow> CreateWindow(const std::string &title, size_t width, size_t height);

    static Ref<Project> CreateProject(ProjectConfig &config);

    static Ref<Project> GetProject() { return project; }

  private:
    inline static Ref<Project> project;
};
}