#pragma once
#include <imgui.h>
#include "function/scene/scene.h"
#include "function/window/window.h"
#include "panels/panel.h"
#include "project/project.h"
#include "engine_extensions/resource/editor_asset_manager.h"

namespace Zafkiel
{
class EditorWindow : public Window
{
  public:
    EditorWindow(const std::string &title, size_t width, size_t height);

    virtual ~EditorWindow() override;

    virtual void OnEvent(Event &event) override;

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