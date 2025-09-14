#include "editor_window.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include "core/meta/serializer/deserialize.h"
#include "core/meta/serializer/serialize.h"
#include "panels/hierarchy_panel.h"
#include "panels/scene_panel.h"
#include "panels/content_browser_panel.h"
#include "panels/properties_panel.h"
#include "function/scene/components.h"
#include "editor.h"
#include "function/engine.h"

namespace Zafkiel
{
EditorWindow::EditorWindow(const std::string &title, size_t width, size_t height)
    : Window(title, width, height)
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

    SetContext(Engine::CreateGraphicsContext(handle));

    ImGui_ImplSDL3_InitForOpenGL(handle, graphicsContext->GetHandle());
    ImGui_ImplOpenGL3_Init("#version 450");

    const std::string &editorConfigText = FileSystem::ReadText("editor_config.yaml");
    EditorConfig editorConfig = Deserialize<EditorConfig>(editorConfigText);

    const std::string &projectConfigText = FileSystem::ReadText(editorConfig.startProjectPath);
    ProjectConfig projectConfig = Deserialize<ProjectConfig>(projectConfigText);
    project = Editor::CreateProject(projectConfig);

    currentScene = Engine::CreateScene();
    Engine::SetCurrentScene(currentScene);
    project->GetAssetManager()->DeserializeAssetRegistry();
    project->GetAssetManager()->SerializeAssetRegistry();

    World &world = currentScene->GetWorld();

    const std::string &worldStr = FileSystem::ReadText(projectConfig.startScene);
    world = Deserialize<World>(worldStr);

    auto scenePanel = MakeRef<ScenePanel>(graphicsContext, project->GetAssetManager());
    scenePanel->SetCurrentScene(currentScene);
    panels.push_back(scenePanel);

    auto hierarchyPanel = MakeRef<HierarchyPanel>();
    hierarchyPanel->SetCurrentScene(currentScene);
    panels.push_back(hierarchyPanel);

    auto propertiesPanel = MakeRef<PropertiesPanel>();
    propertiesPanel->SetCurrentScene(currentScene);
    panels.push_back(propertiesPanel);

    auto contentBroswerPanel = MakeRef<ContentBrowserPanel>();
    contentBroswerPanel->SetCurrentDirectory(project->GetAssetManager()->GetAssetDirectory());
    panels.push_back(contentBroswerPanel);
}

EditorWindow::~EditorWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void EditorWindow::OnEvent(Event &event)
{
    ImGui_ImplSDL3_ProcessEvent(&event.GetHandle());
}

}