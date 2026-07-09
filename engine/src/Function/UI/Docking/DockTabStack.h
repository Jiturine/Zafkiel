#pragma once
#include "Function/UI/Docking/DockTabBar.h"
#include "Function/UI/Docking/DockNode.h"
#include "Function/UI/Panel.h"
#include "Function/UI/Splitter.h"

namespace Zafkiel
{
class DockTabStack : public DockNode
{
  public:
    void Construct();

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

    virtual Reply OnDragOver(WidgetGeometry widgetGeometry, DragDropEvent &event) override;

    virtual Reply OnDragLeave(WidgetGeometry widgetGeometry, DragDropEvent &event) override;

    virtual Reply OnDrop(WidgetGeometry widgetGeometry, DragDropEvent &event) override;

    void ShowTabContent(Ref<Widget> tabContent);

    void OpenTab(const Ref<DockTab> &tab);

    void OnLastTabRemoved();

    Ref<DockTabBar> GetDockTabBar() const
    {
        return dockTabBar;
    }

    virtual Type GetNodeType() const override { return Type::DockTabStack; }

    virtual CleanUpResult CleanUpNodes() override 
    {
        if (dockTabBar->GetTabCount() > 0)
            return CleanUpResult::VisibleTabsUnderNode;
        else 
            return CleanUpResult::NoTabsUnderNode;
    }

  private:
    DockTargetDirection ComputeDockTargetDirection(WidgetGeometry widgetGeometry, vec2 position);

    DockTargetDirection currentDockTargetDirection;

    Ref<DockTabBar> dockTabBar;

    Ref<Panel> panel;

    uint32 tabContentSlotIndex;
};

}