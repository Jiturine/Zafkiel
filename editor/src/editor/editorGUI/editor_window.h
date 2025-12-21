#pragma once
#include "function/scene/scene.h"
#include "function/window/window.h"
#include "editor/panels/panel.h"
#include "editor/project/project.h"
#include "editor/resource/editor_asset_manager.h"

namespace Zafkiel
{
class EditorWindow : public Window
{
  public:
    EditorWindow(const WindowSpecification &spec);

    virtual ~EditorWindow() override;

    virtual void OnEvent(SDL_Event &event) override;
    
    void CreatePanels();
    void DestroyPanels();

    template <typename T>
        requires std::is_base_of_v<Panel, T>
    Ref<T> GetActivePanel()
    {
        for (auto &panel : panels)
        {
            if (panel.Is<T>()) return panel.As<T>();
        }
        return nullptr;
    }

    std::vector<Ref<Panel>> panels;
  private:
    void InitImGui();
};
}
