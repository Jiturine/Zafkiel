#include "function/window/window.h"
#include "core/time/time.h"
#include "editorGUI/editorGUI.h"
#include "editorGUI/editor_window.h"
#include "panels/scene_panel.h"

#include "core/meta/serializer/serialize.h"
#include "core/meta/serializer/deserialize.h"
#include "function/scene/components.h"

#include "core/meta/reflection/any.h"
#include "refl_generate.h"

using namespace Zafkiel;

int main()
{
    ReflectionGenerate::RegisterReflectionInfo();
    Reflection::Register<ProjectConfig>("ProjectConfig")
        .AddProperty(&ProjectConfig::name, "name")
        .AddProperty(&ProjectConfig::startScene, "startScene")
        .AddProperty(&ProjectConfig::assetDirectory, "assetDirectory");

    EditorWindow window("Zafkiel Editor", 1920, 1080);

    float time = Time::Now();
    bool value = false;
    bool dockspace_open = true;

    // clang-format off
const char* str = R"(
testStruct:
  oneProp: 1
  scripts:
    - hello
    - world
  foobar: Bar
)";
    // clang-format on
    UUIDComponent uuid;
    Log::CoreTrace("{}", Serialize(uuid));

    while (!window.ShouldClose())
    {
        float timestep = Time::Now() - time;
        time = Time::Now();

        if (auto scenePanel = window.GetActivePanel<ScenePanel>())
        {
            scenePanel->RenderScene(window.currentScene);
            scenePanel->Update(timestep);
        }

        EditorGUI::StartFrame();
        {
            GUIDockSpace dockspace("Hello DockSpace!", dockspace_open);
            {
                for (auto panel : window.panels)
                {
                    panel->Render();
                }
            }
        }
        EditorGUI::EndFrame();

        window.OnUpdate(timestep);
    }
    return 0;
}