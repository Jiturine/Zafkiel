#include "editor/editorGUI/editor_window.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_vulkan.h"
#include <vulkan/vulkan.hpp>
#include "function/render/backends/vulkan/vulkan_context.h"
#include "function/render/backends/vulkan/vulkan_render_pass.h"
#include "editor/editorGUI/editorGUI.h"
#include "editor/panels/hierarchy_panel.h"
#include "editor/panels/scene_panel.h"
#include "editor/panels/content_browser_panel.h"
#include "editor/panels/toolbar_panel.h"
#include "editor/panels/properties_panel.h"
#include "function/scene/components.h"
#include "function/input/input.h"
#include "function/render/renderer.h"

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
        uint32_t width = event.window.data1;
        uint32_t height = event.window.data2;
        Renderer::Submit([width, height](){
            Renderer::GetGraphicsContext()->Resize(width, height);
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