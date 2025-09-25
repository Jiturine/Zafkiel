#include "editor_window.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include "core/meta/serializer/deserialize.h"
#include "core/meta/serializer/serialize.h"
#include "panels/hierarchy_panel.h"
#include "panels/scene_panel.h"
#include "panels/content_browser_panel.h"
#include "panels/toolbar_panel.h"
#include "panels/properties_panel.h"
#include "function/scene/components.h"
#include "editor.h"
#include "function/engine.h"

namespace Zafkiel
{
EditorWindow::EditorWindow(const std::string &title, size_t width, size_t height)
    : Window(title, width, height)
{
    auto graphicsContext = Engine::CreateGraphicsContext(handle);
    SetContext(graphicsContext);

    InitImGui();

    auto scenePanel = MakeRef<ScenePanel>();
    panels.push_back(scenePanel);

    auto hierarchyPanel = MakeRef<HierarchyPanel>();
    panels.push_back(hierarchyPanel);

    auto propertiesPanel = MakeRef<PropertiesPanel>();
    panels.push_back(propertiesPanel);

    auto contentBroswerPanel = MakeRef<ContentBrowserPanel>();
    panels.push_back(contentBroswerPanel);

    auto toolbarPanel = MakeRef<ToolbarPanel>();
    panels.push_back(toolbarPanel);
}

EditorWindow::~EditorWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void EditorWindow::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;

    ImGui::StyleColorsDark();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    io.Fonts->AddFontFromFileTTF("assets/fonts/HarmonyOS_Sans_SC/HarmonyOS_Sans_SC_Regular.ttf");
    ImGui_ImplSDL3_InitForOpenGL(handle, graphicsContext->GetHandle());
    ImGui_ImplOpenGL3_Init("#version 450");
}

void EditorWindow::OnEvent(Event &event)
{
    ImGui_ImplSDL3_ProcessEvent(&event.GetHandle());
}

}