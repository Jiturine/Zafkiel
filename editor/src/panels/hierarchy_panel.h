#pragma once

#include "function/scene/scene.h"
#include "panel.h"

namespace Zafkiel
{
class HierarchyPanel : public Panel
{
  public:
    HierarchyPanel() {}

    void SetCurrentScene(Ref<Scene> currentScene)
    {
        scene = currentScene;
    }
    virtual void Render() override;
  private:
    void DrawEntityNode(Entity entity);

    Ref<Scene> scene;
};
}