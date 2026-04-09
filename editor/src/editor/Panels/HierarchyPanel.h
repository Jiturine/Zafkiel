#pragma once

#include "Function/Scene/Scene.h"
#include "editor/Panels/Panel.h"

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