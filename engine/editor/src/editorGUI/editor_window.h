#pragma once
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include <imgui.h>
#include "core/window.h"
#include "../panels/panel.h"

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
    std::unique_ptr<T> CreatePanel()
    {
        return std::make_unique<T>(graphicsContext);
    }
};
}