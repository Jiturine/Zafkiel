#pragma once
#include "Function/UI/Docking/DockTab.h"

namespace Zafkiel
{
class DockTabStack;

class DockTabBar : public Widget
{
  public:
    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

    virtual void ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const override;

    virtual vec2 GetDesiredSize() const override;

    virtual Reply OnDragEnter(WidgetGeometry widgetGeometry, DragDropEvent &event) override;

    virtual Reply OnDragOver(WidgetGeometry widgetGeometry, DragDropEvent &event) override;

    virtual Reply OnDragLeave(WidgetGeometry widgetGeometry, DragDropEvent &event) override;

    virtual Reply OnDrop(WidgetGeometry widgetGeometry, DragDropEvent &event) override;

    Reply StartDraggingTab(Ref<DockTab> tabBeingDragged, vec2 tabGrabOffsetFraction, PointerEvent &pointerEvent);

    DockTabBar *SetParentStack(Ref<DockTabStack> stack)
    {
        parentTabStack = stack;
        return this;
    }

    Ref<DockTabStack> GetParentStack() const;

    void BringTabToFront(uint32 index)
    {
        foregroundTabIndex = index;
        RefreshParentContent();
    }

    void BringTabToFront(const Ref<DockTab> &tab)
    {
        BringTabToFront(std::find(tabs.begin(), tabs.end(), tab) - tabs.begin());
    }

    uint32 GetTabCount() const { return tabs.size(); }

    void AddTab(const Ref<DockTab> &tab)
    {
        tabs.push_back(tab);
        tab->SetParentTabBar(this);

        BringTabToFront(tab);
    }

    void RemoveTab(const Ref<DockTab> &tab)
    {
        tabs.erase(std::find(tabs.begin(), tabs.end(), tab));
    }

    void RefreshParentContent();
    
  private:
    static constexpr int32 INDEX_NONE = -1;

    float ComputeDraggedTabOffset(WidgetGeometry allocatedGeometry, const PointerEvent& pointerEvent, vec2 tabGrabOffsetFraction) const;
    
    vec2 ComputeChildSize(vec2 allocatedSize) const;

    uint32 ComputeChildDropIndex(WidgetGeometry allocatedGeometry, Ref<DockTab> inDraggedTab) const;

    std::vector<Ref<DockTab>> tabs;

    Ref<DockTab> draggedTab;

    WeakRef<DockTabStack> parentTabStack;

    float draggedTabOffset;

    vec2 tabGrabOffsetFraction;

    int32 foregroundTabIndex = INDEX_NONE;
};

}