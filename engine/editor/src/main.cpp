#include "core/window.h"
#include "core/time.h"
#include "editorGUI/editorGUI.h"
#include "editorGUI/editor_window.h"
#include "panels/scene_panel.h"

using namespace Zafkiel;

int main()
{
    EditorWindow window("Zafkiel Editor", 1920, 1080);

    float time = Time::Now();
    bool value = false;
    bool dockspace_open = true;
    std::unique_ptr<ScenePanel> scenePanel = window.CreatePanel<ScenePanel>();

    while (!window.ShouldClose())
    {
        float timestep = Time::Now() - time;
        time = Time::Now();

        scenePanel->RenderScene();
        scenePanel->Update(timestep);

        EditorGUI::StartFrame();
        {
            GUIDockSpace dockspace("Hello DockSpace!", dockspace_open);
            {
                GUIWindow testWindow("Test Window");

                scenePanel->Render();

                EditorGUI().Text("{}", time).CheckBox("Check me", value).Button("Close", []() { Log::CoreInfo("Hello!"); });
            }
        }
        EditorGUI::EndFrame();

        window.OnUpdate(timestep);
    }
    return 0;
}