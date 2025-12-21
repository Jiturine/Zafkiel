#include "toolbar_panel.h"
#include "core/meta/serializer/yaml_serializer.h"
#include "editor/editorGUI/editorGUI.h"
#include "function/scene/components.h"
#include "function/script/script_engine.h"
#include "function/scene/scene.h"
#include "editor/function/script/editor_script_engine.h"
#include "function/render/renderer.h"

namespace Zafkiel
{

ToolbarPanel::ToolbarPanel()
{
    // playButton = Renderer::Instance().GetGraphicsContext()->CreateTexture2D("assets/textures/play_button.png");
    // stopButton = Renderer::Instance().GetGraphicsContext()->CreateTexture2D("assets/textures/stop_button.png");
}

void ToolbarPanel::Render()
{
    GUIWindow toolbarPanel("##toolbar");

    // auto handle = isPlaying ? stopButton->GetRendererID() : playButton->GetRendererID();

    // if (ImGui::ImageButton("##play_or_stop", handle, ImVec2(20, 20)))
    // {
        // isPlaying = !isPlaying;
        // if (isPlaying)
        // {
        //     auto sceneData = Editor::GetEditorScene()->Serialize();
        //     EditorScriptEngine::OnRuntimeInit();
        //     Ref<Scene> newScene = CreateRef<Scene>(sceneData);
        //     Engine::SetActiveScene(newScene);
        //     Editor::SetRuntimeScene(newScene);
        //     // newScene->Deserialize(sceneData);
        //     EditorScriptEngine::OnRuntimeStart();
        // }
        // else
        // {
        //     Engine::SetActiveScene(Editor::GetEditorScene());
        //     EditorScriptEngine::OnRuntimeStop();
        // }
    // }
}
}
