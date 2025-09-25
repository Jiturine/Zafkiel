#include "editor.h"
#include "editorGUI/editor_window.h"
#include "script/editor_script_engine.h"

namespace Zafkiel
{
Ref<EditorScriptEngine> Editor::scriptEngine;
Ref<Scene> Editor::editorScene;
Ref<Scene> Editor::runtimeScene;
Ref<Project> Editor::project;
Ref<EditorWindow> Editor::editorWindow;
Entity Editor::selectedEntity;

Ref<EditorWindow> Editor::CreateWindow(const std::string &title, size_t width, size_t height)
{
    editorWindow = MakeRef<EditorWindow>(title, width, height);
    return editorWindow;
}

Ref<Project> Editor::CreateProject(ProjectConfig &config)
{
    project = MakeRef<Project>(config);
    return project;
}

Ref<EditorScriptEngine> Editor::CreateScriptEngine()
{
    scriptEngine = MakeRef<EditorScriptEngine>("ScriptCore.dll");
    return scriptEngine;
}

Ref<EditorScriptEngine> Editor::GetScriptEngine()
{
    return scriptEngine;
}

void Editor::SetEditorScene(const Ref<Scene> &scene)
{
    editorScene = scene;
}

Ref<Scene> Editor::GetEditorScene()
{
    return editorScene;
}

void Editor::SetRuntimeScene(const Ref<Scene> &scene)
{
    runtimeScene = scene;
}

Ref<Scene> Editor::GetRuntimeScene()
{
    return runtimeScene;
}

Entity Editor::GetSelectedEntity()
{
    if (!Engine::GetActiveScene()->GetWorld().HasEntity(selectedEntity))
        selectedEntity = Entity();
    return selectedEntity;
}

void Editor::SetSelectedEntity(Entity entity)
{
    selectedEntity = entity;
}

}
