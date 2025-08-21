#include "core/window.h"
#include "core/time.h"
#include "editorGUI/editorGUI.h"
#include "editorGUI/editor_window.h"
using namespace Zafkiel;

int main()
{
    EditorWindow window("Zafkiel Editor", 1920, 1080);

    float time = Time::Now();
    bool value = false;
    bool dockspace_open = true;
    while (!window.ShouldClose())
    {
        float timestep = Time::Now() - time;
        time = Time::Now();

        EditorGUI::StartFrame();
        {
            GUIDockSpace dockspace("Hello DockSpace!", dockspace_open);
            {
                GUIWindow testWindow("Test Window");

                EditorGUI().Text("{}", time).CheckBox("Check me", value).Button("Close", []() { Log::CoreInfo("Hello!"); });
            }
        }
        EditorGUI::EndFrame();

        window.OnUpdate(timestep);
    }
    return 0;
}