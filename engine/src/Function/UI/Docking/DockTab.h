#pragma once
#include "Function/UI/Slot.h"
#include "Function/UI/Border.h"
#include "Function/UI/Text.h"

namespace Zafkiel
{
class DockTabBar;

// 可拖动的，对应一个Content的小标签
class DockTab : public Border
{
  public:
    void Construct(const std::wstring &titleStr);

    void SetTabContent(Ref<Widget> widget)
    {
        tabContent = widget;
    }

    void SetTabTitle(const std::wstring &str)
    {
        tabTitle->SetText(str);
    }

    Ref<Widget> GetContent() const
    {
        return tabContent;
    }

    Ref<DockTabBar> GetParent() const;

    Reply OnMouseButtonDown(WidgetGeometry widgetGeometry, PointerEvent &event) override;

    virtual Reply OnDragDetected(WidgetGeometry widgetGeometry, PointerEvent &event, vec2 startPosition) override;

  private:
    friend class DockTabBar;
    void SetParentTabBar(Ref<DockTabBar> dockTabBar)
    {
        parentTabBar = dockTabBar;
    }

    Ref<Widget> tabContent;

    std::wstring tabTitleStr;

    Ref<Text> tabTitle;

    WeakRef<DockTabBar> parentTabBar;
    
};

}