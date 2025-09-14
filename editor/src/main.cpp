#include "function/window/window.h"
#include "core/time/time.h"
#include "editorGUI/editorGUI.h"
#include "editorGUI/editor_window.h"
#include "panels/scene_panel.h"

#include "core/meta/serializer/serialize.h"
#include "core/meta/serializer/deserialize.h"
#include "function/scene/components.h"

#include "core/meta/reflection/any.h"
#include "engine_refl_generate.h"
#include "editor_refl_generate.h"

#include "editor.h"
#include "function/engine.h"

using namespace Zafkiel;

int main()
{
    Reflection::RegisterEditor();
    Reflection::RegisterEngine();

    Ref<EditorWindow> window = Editor::CreateWindow("Zafkiel Editor", 1920, 1080);

    float time = Time::Now();

    while (!window->ShouldClose())
    {
        float timestep = Time::Now() - time;
        time = Time::Now();

        if (auto scenePanel = window->GetActivePanel<ScenePanel>())
        {
            scenePanel->RenderScene(window->currentScene);
            scenePanel->Update(timestep);
        }

        EditorGUI::StartFrame();
        {
            GUIDockSpace dockspace("Hello DockSpace!");
            {
                for (auto panel : window->panels)
                {
                    panel->Render();
                }
            }
        }
        EditorGUI::EndFrame();

        window->OnUpdate(timestep);
    }
    return 0;
}