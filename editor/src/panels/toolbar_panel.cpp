#include "toolbar_panel.h"
#include "core/meta/serializer/deserialize.h"
#include "editorGUI/editorGUI.h"
#include "function/engine.h"
#include "editor.h"
#include "function/scene/components.h"
#include "function/script/script_engine.h"
#include "function/scene/scene.h"
#include "script/editor_script_engine.h"

namespace Zafkiel
{

ToolbarPanel::ToolbarPanel()
{
    playButton = Engine::GetGraphicsContext()->CreateTexture2D("assets/textures/play_button.png");
    stopButton = Engine::GetGraphicsContext()->CreateTexture2D("assets/textures/stop_button.png");
}

void ToolbarPanel::Render()
{
    GUIWindow toolbarPanel("##toolbar");

    auto handle = isPlaying ? stopButton->GetRendererID() : playButton->GetRendererID();

    if (ImGui::ImageButton("##play_or_stop", handle, ImVec2(20, 20)))
    {
        isPlaying = !isPlaying;
        if (isPlaying)
        {
            auto worldData = Serialize(Editor::GetEditorScene()->GetWorld());
            Ref<Scene> newScene = MakeRef<Scene>();
            Engine::SetActiveScene(newScene);
            Editor::SetRuntimeScene(newScene);
            Editor::GetScriptEngine()->OnRuntimeInit();
            Deserialize<World>(worldData, newScene->GetWorld());
            Editor::GetScriptEngine()->OnRuntimeStart();
        }
        else
        {
            Engine::SetActiveScene(Editor::GetEditorScene());
            Editor::GetScriptEngine()->OnRuntimeStop();
        }
    }
}
}
