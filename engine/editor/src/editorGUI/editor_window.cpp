#include "editor_window.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include "core/meta/serializer/deserialize.h"
#include "core/meta/serializer/serialize.h"
#include "panels/hierarchy_panel.h"
#include "panels/scene_panel.h"
#include "panels/properties_panel.h"
#include "function/scene/components.h"

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

    ImGui_ImplSDL3_InitForOpenGL(handle, graphicsContext->GetHandle());
    ImGui_ImplOpenGL3_Init("#version 450");

    const std::string &configText = FileSystem::ReadText("config.yaml");
    ProjectConfig config = Deserialize<ProjectConfig>(configText);
    project = MakeRef<Project>(config, graphicsContext);

    currentScene = MakeRef<Scene>();
    AssetHandle handle = project->GetAssetManager()->ImportAsset("textures/furina.png");

    World &world = currentScene->GetWorld();
    world.SpawnEntity(
        TagComponent{"Object1", "Object"},
        TransformComponent{vec3(1.0f), vec3(0.0f), vec3(1.0f)},
        SpriteRendererComponent{vec4(1.0f), handle});
    world.SpawnEntity(
        TagComponent{"Object2", "Object"},
        TransformComponent{vec3(3.0f, 1.0f, 1.0f), vec3(0.0f), vec3(1.0f)},
        SpriteRendererComponent{vec4(1.0f, 0.8f, 0.7f, 1.0f), handle});
    world.SpawnEntity(
        TagComponent{"Object3", "Object"},
        TransformComponent{vec3(5.0f, 1.0f, 1.0f), vec3(0.0f), vec3(1.0f)},
        SpriteRendererComponent{vec4(0.7f, 0.9f, 0.8f, 1.0f), handle});

    auto scenePanel = MakeRef<ScenePanel>(graphicsContext, handle, project->GetAssetManager());
    scenePanel->SetCurrentScene(currentScene);
    panels.push_back(scenePanel);

    auto hierarchyPanel = MakeRef<HierarchyPanel>();
    hierarchyPanel->SetCurrentScene(currentScene);
    panels.push_back(hierarchyPanel);

    auto propertiesPanel = MakeRef<PropertiesPanel>();
    propertiesPanel->SetCurrentScene(currentScene);
    panels.push_back(propertiesPanel);
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