#include "Function/UI/Docking/DockTabBar.h"
#include "Function/UI/Docking/DockDragDropOperation.h"
#include "Function/UI/Docking/DockTabStack.h"
#include "Function/UI/WidgetGeometryList.h"

namespace Zafkiel
{

Ref<DockTabStack> DockTabBar::GetParentStack() const
{
    return parentTabStack.Lock();
}

uint32 DockTabBar::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) 
{
    std::vector<ArrangedWidget> arrangedChildren;
    ArrangeChildren(allocatedGeometry, arrangedChildren);

    widgetGeometryList.AddWidgetGeometry(this, allocatedGeometry, layerId);

    uint32 maxLayerId = layerId;
    for (auto &arrangedChild : arrangedChildren)
    {
        uint32 curChildMaxLayerId = arrangedChild.widget->Draw(arrangedChild.widgetGeometry, drawElementList, widgetGeometryList, this, layerId);
        maxLayerId = std::max(maxLayerId, curChildMaxLayerId);
    }

    return maxLayerId;
}

void DockTabBar::ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const 
{
    int draggedTabIndex = INDEX_NONE;
    if (draggedTab && std::find(tabs.begin(), tabs.end(), draggedTab) != tabs.end())
    {
        draggedTabIndex = ComputeChildDropIndex(allocatedGeometry, draggedTab);
    }

    vec2 childSize = ComputeChildSize(allocatedGeometry.size);
    float xOffset = 0.0f;
    
    for (uint32 i = 0; i < tabs.size(); i++)
    {
        if (draggedTabIndex == i) xOffset += childSize.x;
        
        if (tabs[i] == draggedTab) continue;
        
        arrangedChildren.emplace_back(tabs[i].get(), WidgetGeometry{childSize, allocatedGeometry.position + vec2(xOffset, 0)});

        xOffset += childSize.x;
    }

    if (draggedTabIndex != INDEX_NONE)
    {
        arrangedChildren.emplace_back(draggedTab.get(), WidgetGeometry{childSize, allocatedGeometry.position + vec2(draggedTabOffset, 0)});
    }
}

Reply DockTabBar::OnDragEnter(WidgetGeometry widgetGeometry, DragDropEvent &event) 
{
    auto dragDropOperation = event.GetOperation();
    if (dragDropOperation.Is<DockDragDropOperation>())
    {
        auto dockDragDropOperation = dragDropOperation.As<DockDragDropOperation>();

        dockDragDropOperation->PinTab();

        draggedTab = dockDragDropOperation->GetDraggedTab();

        if (std::find(tabs.begin(), tabs.end(), draggedTab) == tabs.end())
        {
            tabs.push_back(draggedTab);
        }
        draggedTab->SetParentTabBar(this);

        tabGrabOffsetFraction = dockDragDropOperation->GetTabGrabOffsetFraction();

        return Reply::Handled();
    }

    return Reply::Unhandled();
}

Reply DockTabBar::OnDragOver(WidgetGeometry widgetGeometry, DragDropEvent &event) 
{
    auto dragDropOperation = event.GetOperation();
    if (dragDropOperation.Is<DockDragDropOperation>())
    {
        auto dockDragDropOperation = dragDropOperation.As<DockDragDropOperation>();

        draggedTabOffset = ComputeDraggedTabOffset(widgetGeometry, event, tabGrabOffsetFraction);

        return Reply::Unhandled(); // 让DockTabStack计算DockTargetDirection，此时为None
    }

    return Reply::Unhandled();
}

Reply DockTabBar::OnDragLeave(WidgetGeometry widgetGeometry, DragDropEvent &event) 
{
    auto dragDropOperation = event.GetOperation();
    if (dragDropOperation.Is<DockDragDropOperation>())
    {
        auto dockDragDropOperation = dragDropOperation.As<DockDragDropOperation>();

        assert(dockDragDropOperation->GetDraggedTab() == draggedTab);

        uint32 lastForegroundTabIndex = std::find(tabs.begin(), tabs.end(), draggedTab) - tabs.begin();

        if (tabs.size() > 1)
        {
            if (lastForegroundTabIndex + 1 < tabs.size())
            {
                BringTabToFront(lastForegroundTabIndex + 1);
            }
            else
            {
                BringTabToFront(std::max(lastForegroundTabIndex - 1, 0u));
            }
        }

        RemoveTab(draggedTab);

        draggedTab->SetParentTabBar(nullptr);
        draggedTab = nullptr;

        if (tabs.size() == 0)
        {
            parentTabStack->OnLastTabRemoved();
        }

        dockDragDropOperation->FloatTab(event);
        
        return Reply::Handled();
    }

    return Reply::Unhandled();
}

Reply DockTabBar::OnDrop(WidgetGeometry widgetGeometry, DragDropEvent &event)
{
    auto dragDropOperation = event.GetOperation();
    if (dragDropOperation.Is<DockDragDropOperation>())
    {
        auto dockDragDropOperation = dragDropOperation.As<DockDragDropOperation>();

        if (draggedTab)
        {
            uint32 targetIndex = ComputeChildDropIndex(widgetGeometry, draggedTab);
            auto it = std::find(tabs.begin(), tabs.end(), draggedTab);
            if (it != tabs.end())
            {
                tabs.erase(it);
                targetIndex = std::min(targetIndex, (uint32)tabs.size());
                tabs.insert(tabs.begin() + targetIndex, draggedTab);
                BringTabToFront(targetIndex);
            }

            draggedTab = nullptr;
        }

        return Reply::Handled();
    }

    return Reply::Unhandled();
}

vec2 DockTabBar::GetDesiredSize() const 
{
    vec2 desiredSize(0.0f);
    for (auto &tab : tabs)
    {
        vec2 tabDesiredSize = tab->GetDesiredSize();
        desiredSize.x += tabDesiredSize.x;
        desiredSize.y = std::max(desiredSize.y, tabDesiredSize.y);
    }
    return desiredSize;
}

Reply DockTabBar::StartDraggingTab(Ref<DockTab> tabBeingDragged, vec2 inTabGrabOffsetFraction, PointerEvent &pointerEvent)
{
    Ref<DockDragDropOperation> operation = CreateRef<DockDragDropOperation>(tabBeingDragged, inTabGrabOffsetFraction, parentTabStack->GetPaintGeometry().size);

    tabGrabOffsetFraction = inTabGrabOffsetFraction;

    draggedTabOffset = ComputeDraggedTabOffset(pointerEvent.GetWidgetPath()->FindArrangedWidget(this).widgetGeometry, pointerEvent, inTabGrabOffsetFraction);

    draggedTab = tabBeingDragged;

    tabBeingDragged->SetVisibility(Visibility::HitTestInvisible);

    tabs.erase(std::find(tabs.begin(), tabs.end(), tabBeingDragged));

    return Reply::Handled().BeginDragDrop(operation);
}

vec2 DockTabBar::ComputeChildSize(vec2 allocatedSize) const
{
    float childSizeX = allocatedSize.x / tabs.size();
    float childSizeY = allocatedSize.y;
    return vec2(childSizeX, childSizeY);
}

float DockTabBar::ComputeDraggedTabOffset(WidgetGeometry allocatedGeometry, const PointerEvent& pointerEvent, vec2 tabGrabOffsetFraction) const
{
	vec2 computedChildSize = ComputeChildSize(allocatedGeometry.size);

	return (pointerEvent.GetPosition().x - allocatedGeometry.position.x) - tabGrabOffsetFraction.x * computedChildSize.x;
}

uint32 DockTabBar::ComputeChildDropIndex(WidgetGeometry allocatedGeometry, Ref<DockTab> inDraggedTab) const
{
    float childWidth = ComputeChildSize(allocatedGeometry.size).x;
    float draggedTabCenter = draggedTabOffset + childWidth / 2;
    uint32 targetIndex = 0;
    float xOffset = -childWidth / 2;
    for (uint32 i = 0; i < tabs.size(); i++)
    {
        xOffset += childWidth;
        if (xOffset > draggedTabCenter) break;

        targetIndex++;
    }
    return targetIndex;
}

void DockTabBar::RefreshParentContent()
{
    if (tabs.size() > 0 && foregroundTabIndex != INDEX_NONE)
    {
        auto &foregroundTab = tabs[foregroundTabIndex];
        parentTabStack->ShowTabContent(foregroundTab->GetContent());
    }
}

}