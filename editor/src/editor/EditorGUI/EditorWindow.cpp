#include "editor/EditorGUI/EditorWindow.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_vulkan.h"
#include <vulkan/vulkan.hpp>
#include "editor/EditorGUI/EditorGUI.h"
#include "editor/Panels/HierarchyPanel.h"
#include "editor/Panels/ScenePanel.h"
#include "editor/Panels/ContentBrowserPanel.h"
#include "editor/Panels/ToolbarPanel.h"
#include "editor/Panels/PropertiesPanel.h"
#include "Function/Scene/Components.h"
#include "Function/Input/Input.h"
#include "Function/Render/Renderer.h"

namespace Zafkiel
{
EditorWindow::EditorWindow(const WindowSpecification &spec)
    : Window(spec)
{

}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::CreatePanels()
{
    auto scenePanel = CreateRef<ScenePanel>();
    panels.push_back(scenePanel);

    // auto hierarchyPanel = CreateRef<HierarchyPanel>();
    // panels.push_back(hierarchyPanel);

    // auto propertiesPanel = CreateRef<PropertiesPanel>();
    // panels.push_back(propertiesPanel);

    // auto contentBroswerPanel = CreateRef<ContentBrowserPanel>();
    // panels.push_back(contentBroswerPanel);

    // auto toolbarPanel = CreateRef<ToolbarPanel>();
    // panels.push_back(toolbarPanel)
}

void EditorWindow::DestroyPanels()
{
    for (auto &panel : panels)
    {
        panel = nullptr;
    }
}

void EditorWindow::OnEvent(SDL_Event &event)
{
    Window::OnEvent(event);
    ImGui_ImplSDL3_ProcessEvent(&event);
    if (event.type == SDL_EVENT_WINDOW_RESIZED)
    {
        uint32 width = event.window.data1;
        uint32 height = event.window.data2;
        Renderer::Submit([width, height](){
            GlobalRHI->GetGraphicsContext()->Resize(width, height);
        });
    }
    if (Input::IsKeyDown(KeyCode::LCtrl) && Input::IsKeyPressed(KeyCode::S))
    {
        // Ref<Scene> scene = Editor::GetEditorScene();
        // std::string sceneData = scene->Serialize();
        // auto sceneMetadata = EditorAssetManager::GetAssetMetadata(scene->handle).As<EditorSceneMetadata>();
        // FileSystem::SaveText(EditorAssetManager::GetAssetDirectory() / sceneMetadata->filePath, sceneData);
    }
    if (event.type == SDL_EVENT_DROP_FILE)
    {
        // Path filePath = event.drop.data;
        // if (filePath.extension() == ".zaf")
        //     Editor::OpenScene(filePath);
    }
}

}