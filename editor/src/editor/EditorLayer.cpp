#include "editor/EditorLayer.h"
#include "editor/Function/Scene/EditorSceneManager.h"
#include "editor/Resource/EditorAssetManager.h"
#include "Core/Time/Time.h"
#include "editor/EditorGUI/EditorGUI.h"
#include "editor/EditorGUI/EditorWindow.h"
#include "Function/Render/Renderer.h"
#include "editor/Panels/ContentBrowserPanel.h"
#include "editor/Panels/ScenePanel.h"

#include "Function/Scene/Components.h"

#include "Core/Meta/Serializer/YamlSerializer.h"

#include "Core/Meta/Reflection/Any.h"
#include "EngineReflGenerate.h"
#include "EditorReflGenerate.h"

#include "Function/Input/Input.h"
#include "editor/Function/Script/EditorScriptEngine.h"
#include "Function/Script/ScriptGlue.h"

#include <filewatch.hpp>
#include "Core/Application/Application.h"
#include "editor/Project/ProjectManager.h"
#include "Resource/AssetManager.h"

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
    EditorAssetManager::Instance().SetAssetDirectory(project->GetAssetDirectory());
    // EditorAssetManager::Instance().LoadBuiltinShader();
    EditorAssetManager::Instance().RegisterAssets();

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
