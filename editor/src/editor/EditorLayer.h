#pragma once
#include "Core/Application/Application.h"
#include "editor/EditorGUI/EditorWindow.h"

namespace Zafkiel 
{

struct [[refl]] EditorConfig
{
    std::string startProjectPath;
};

class EditorLayer : public Layer
{
  public:
    EditorLayer() : Layer("Editor Layer") {}
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float timestep) override;
  private:
    Scope<EditorWindow> window;
};

}