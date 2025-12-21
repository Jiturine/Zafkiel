#pragma once

#include "function/scene/scene.h"
#include "panel.h"

namespace Zafkiel
{
class HierarchyPanel : public Panel
{
  public:
    HierarchyPanel() {}
    virtual void Render() override;
  private:
    void DrawEntityNode(Entity entity);
};
}