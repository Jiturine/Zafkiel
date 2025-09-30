#pragma once

#include "platform/filesystem/filesystem.h"
#include "project/project.h"
#include "core/meta/reflection/refl.h"
#include "engine.h"

namespace Zafkiel
{

struct [[refl]] EditorConfig
{
    Path startProjectPath;
};

class EditorWindow;

class EditorScriptEngine;

class Scene;

class Entity;

class Editor
{
  public:
    static Ref<EditorWindow> CreateWindow(const std::string &title, size_t width, size_t height);

    static Ref<Project> CreateProject(ProjectConfig &config);

    static Ref<Project> GetProject() { return project; }

    static Ref<EditorScriptEngine> CreateScriptEngine();

    static Ref<EditorScriptEngine> GetScriptEngine();

    static void SetEditorScene(const Ref<Scene> &scene);

    static Ref<Scene> GetEditorScene();

    static void SetRuntimeScene(const Ref<Scene> &scene);

    static Ref<Scene> GetRuntimeScene();

    static Entity GetSelectedEntity();

    static void SetSelectedEntity(Entity entity);

  private:
    static Ref<EditorWindow> editorWindow;

    static Ref<Project> project;

    static Ref<EditorScriptEngine> scriptEngine;

    static Ref<Scene> editorScene;

    static Ref<Scene> runtimeScene;

    static Entity selectedEntity;
};
}