#include "function/window/window.h"
#include "core/time/time.h"
#include "editorGUI/editorGUI.h"
#include "editorGUI/editor_window.h"
#include "panels/scene_panel.h"

#include "core/meta/serializer/serialize.h"

using namespace Zafkiel;

int main()
{
    {
        EditorWindow window("Zafkiel Editor", 1920, 1080);

        float time = Time::Now();
        bool value = false;
        bool dockspace_open = true;

        //-----------debug------------
        // testStruct ins{1.0f, 2.0f, 3.5f};
        // std::string strOut = Serialize(ins);
        // Log::CoreInfo("{}", strOut);
        //-----------debug------------

        while (!window.ShouldClose())
        {
            float timestep = Time::Now() - time;
            time = Time::Now();

            if (auto scenePanel = window.GetActivePanel<ScenePanel>())
            {
                scenePanel->RenderScene();
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
    }
    return 0;
}