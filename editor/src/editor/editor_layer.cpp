#include "editor/editor_layer.h"
#include "editor/function/scene/editor_scene_manager.h"
#include "editor/resource/editor_asset_manager.h"
#include "function/render/graphics_context.h"
#include "function/render/model.h"
#include "core/time/time.h"
#include "editor/editorGUI/editorGUI.h"
#include "editor/editorGUI/editor_window.h"
#include "function/render/renderer.h"
#include "editor/panels/content_browser_panel.h"
#include "editor/panels/scene_panel.h"

#include "function/scene/components.h"

#include "core/meta/serializer/yaml_serializer.h"

#include "core/meta/reflection/any.h"
#include "engine_refl_generate.h"
#include "editor_refl_generate.h"

#include "function/input/input.h"
#include "editor/function/script/editor_script_engine.h"
#include "function/script/script_glue.h"

#include <filewatch.hpp>
#include "core/application/application.h"
#include "editor/project/project_manager.h"
#include "resource/asset_manager.h"

namespace Zafkiel
{
void EditorLayer::OnAttach()
{
    Reflection::RegisterEditor();
    Reflection::RegisterEngine();
    ScriptGlue::RegisterComponents();

    // window只与渲染相关
    WindowSpecification spec
    {
        .graphicsAPI = GraphicsAPI::Vulkan,
        .title = "Zafkiel Editor",
        .width = 1920,
        .height = 1080
    };
    window = CreateScope<EditorWindow>(spec);

    Renderer::Init(GraphicsAPI::Vulkan, *window);

    // 从文件读取项目
    const std::string &editorConfigText = FileSystem::ReadText("editor_config.yaml");
    EditorConfig editorConfig = Deserialize<EditorConfig>(editorConfigText);

    ProjectManager::Init();
    const std::string &projectConfigText = FileSystem::ReadText(editorConfig.startProjectPath);
    ProjectConfig projectConfig = Deserialize<ProjectConfig>(projectConfigText);
    auto project = ProjectManager::CreateProject(projectConfig);

    EditorAssetManager::Init();
    EditorAssetManager::SetAssetDirectory(project->GetAssetDirectory());
    EditorAssetManager::LoadBuiltinShader();
    EditorAssetManager::RegisterAssets();

    // EditorAssetManager::ImportAsset("models/sponza_obj_version/sponza.obj");
    // EditorAssetManager::ImportAsset("models/sponza_gltf_version/scene.gltf");
    // EditorAssetManager::ImportAsset("models/backpack/backpack.obj");

    // EditorAssetManager::GetAsset(17342462340854182166);

    // window->GetActivePanel<ContentBrowserPanel>()->SetCurrentDirectory(project->GetAssetDirectory());

    // 设置脚本引擎
    EditorScriptEngine::Init();

    EditorScriptEngine::CreateEditorDomain();
    EditorScriptEngine::LoadEditorCoreAssembly();
    EditorScriptEngine::CompileScripts();
    EditorScriptEngine::LoadEditorAppAssembly();

    EditorScriptEngine::WatchScriptFiles(project->GetAssetDirectory() / "scripts");

    // 从文件创建开始场景
    // EditorSceneManager::OpenScene(project->GetStartSceneDirectory());

    // auto model = project->GetAssetManager()->LoadAsset(9275989746607182521).As<Model>();

    // Engine::GetActiveScene()->GetWorld().InstantiateModel(model->handle);
}

void EditorLayer::OnDetach()
{
    EditorScriptEngine::Destroy();
    EditorAssetManager::Destroy();
    ProjectManager::Destroy();
    Renderer::Destroy();
}

void EditorLayer::OnUpdate(float timestep)
{
    if(window->ShouldClose()) Application::Instance().Exit();

    Input::ClearState();
    window->PollEvents();

    // 逻辑更新
    Application::ExecuteMainThreadQueue();

    if (EditorScriptEngine::IsRuntime())
    {
        EditorScriptEngine::OnRuntimeUpdate(timestep);
    }

    // 渲染更新
    // Renderer::GetGraphicsContext()->Clear();

    // if (auto scenePanel = window->GetActivePanel<ScenePanel>())
    // {
    //     scenePanel->RenderScene();
    // }

    EditorGUI::BeginFrame();
    {
        GUIDockSpace dockspace("Hello DockSpace!");
        {
            // for (auto panel : window->panels)
            // {
            //     panel->Render();
            // }
            {
                GUIWindow debugWindow("Debug");
                // EditorGUI().Button("Serialize Scene", []() {
                //     Log::Trace("{}", Engine::GetActiveScene()->Serialize());
                // });
                // EditorGUI().Button("Reload AppAssembly", []() {
                //     Editor::GetScriptEngine()->ReloadEditorDomain();
                // });
                // static std::string handleInput;
                // EditorGUI().InputText("Model Handle", handleInput);
                // EditorGUI().Button("Instantiate Model", [&]() {
                //     Engine::GetActiveScene()->GetWorld().InstantiateModel(std::stoull(handleInput));
                // });
            }
        }
    }
    EditorGUI::EndFrame();

    // 渲染后更新，用到imgui的事件
    if (auto scenePanel = window->GetActivePanel<ScenePanel>())
    {
        scenePanel->Update(timestep);
    }
}

}
