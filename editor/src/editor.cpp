#include "editor.h"
#include "editorGUI/editor_window.h"

namespace Zafkiel
{

Ref<EditorWindow> Editor::CreateWindow(const std::string &title, size_t width, size_t height)
{
    return MakeRef<EditorWindow>(title, width, height);
}

Ref<Project> Editor::CreateProject(ProjectConfig &config)
{
    project = MakeRef<Project>(config);
    return project;
}
}
