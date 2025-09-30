#include "function/render/model.h"
#include "function/window/window.h"
#include "core/time/time.h"
#include "editorGUI/editorGUI.h"
#include "editorGUI/editor_window.h"
#include "panels/content_browser_panel.h"
#include "panels/scene_panel.h"

#include "core/meta/serializer/serialize.h"
#include "core/meta/serializer/deserialize.h"
#include "function/scene/components.h"

#include "core/meta/reflection/any.h"
#include "engine_refl_generate.h"
#include "editor_refl_generate.h"

#include "editor.h"
#include "engine.h"
#include "function/input/input.h"
#include "engine_extensions/function/script/editor_script_engine.h"
#include "function/script/script_glue.h"

#include <filewatch.hpp>

using namespace Zafkiel;

int main()
{
    Reflection::RegisterEditor();
    Reflection::RegisterEngine();
    ScriptGlue::RegisterComponents();

    // window只与渲染相关
    auto window = Editor::CreateWindow("Zafkiel Editor", 1920, 1080);

    // 从文件读取项目
    const std::string &editorConfigText = FileSystem::ReadText("editor_config.yaml");
    EditorConfig editorConfig = Deserialize<EditorConfig>(editorConfigText);

    const std::string &projectConfigText = FileSystem::ReadText(editorConfig.startProjectPath);
    ProjectConfig projectConfig = Deserialize<ProjectConfig>(projectConfigText);
    auto project = Editor::CreateProject(projectConfig);
    project->GetAssetManager()->DeserializeAssetRegistry();

    Engine::SetAssetManager(project->GetAssetManager());

    // project->GetAssetManager()->ImportAsset("models/african_head.obj");
    // project->GetAssetManager()->SerializeAssetRegistry();

    window->GetActivePanel<ContentBrowserPanel>()->SetCurrentDirectory(project->GetAssetDirectory());

    // 设置脚本引擎
    auto scriptEngine = Editor::CreateScriptEngine();
    Engine::SetScriptEngine(scriptEngine);

    scriptEngine->CreateEditorDomain();
    scriptEngine->LoadEditorCoreAssembly();
    scriptEngine->CompileScripts();
    scriptEngine->LoadEditorAppAssembly();

    scriptEngine->WatchScriptFiles(project->GetAssetDirectory() / "scripts");

    // 从文件创建开始场景
    auto scene = Engine::CreateScene();
    Engine::SetActiveScene(scene);
    Editor::SetEditorScene(scene);

    World &world = scene->GetWorld();
    const std::string &worldStr = FileSystem::ReadText(project->GetStartSceneDirectory());
    Deserialize<World>(worldStr, world);

    world.InstantiateModel(2971917492089503043);

    float time = Time::Now();

    while (!window->ShouldClose())
    {
        float timestep = Time::Now() - time;
        time = Time::Now();

        Input::ClearState();
        window->PollEvents();

        // 逻辑更新
        Engine::ExecuteMainThreadQueue();
        if (scriptEngine->isRuntime)
        {
            scriptEngine->OnRuntimeUpdate(timestep);
        }

        // 渲染更新
        Engine::GetGraphicsContext()->Clear();

        if (auto scenePanel = window->GetActivePanel<ScenePanel>())
        {
            scenePanel->RenderScene(Engine::GetActiveScene());
        }

        EditorGUI::StartFrame();
        {
            GUIDockSpace dockspace("Hello DockSpace!");
            {
                for (auto panel : window->panels)
                {
                    panel->Render();
                }
                {
                    GUIWindow debugWindow("Debug");
                    EditorGUI().Button("Serialize Scene", []() {
                        Log::CoreTrace("{}", Serialize(Engine::GetActiveScene()->GetWorld()));
                    });
                    EditorGUI().Button("Reload AppAssembly", []() {
                        Editor::GetScriptEngine()->ReloadEditorDomain();
                    });
                }
            }
        }
        EditorGUI::EndFrame();

        // 渲染后更新，用到imgui的事件
        if (auto scenePanel = window->GetActivePanel<ScenePanel>())
        {
            scenePanel->Update(timestep);
        }

        window->SwapBuffers();
    }

    project->GetAssetManager()->SerializeAssetRegistry();

    return 0;
}