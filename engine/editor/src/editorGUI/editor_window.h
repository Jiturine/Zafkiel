#pragma once
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include <imgui.h>
#include "core/window.h"

namespace Zafkiel
{
class EditorWindow : public Window
{
  public:
    EditorWindow(const std::string &title, size_t width, size_t height);

    virtual ~EditorWindow() override;

    virtual void OnEvent(Event &event) override;
};
}