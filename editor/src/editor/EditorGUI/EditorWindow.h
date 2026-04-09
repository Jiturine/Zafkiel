#pragma once
#include "Function/Scene/Scene.h"
#include "Function/Window/Window.h"
#include "editor/Panels/Panel.h"
#include "editor/Project/Project.h"
#include "editor/Resource/EditorAssetManager.h"

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
