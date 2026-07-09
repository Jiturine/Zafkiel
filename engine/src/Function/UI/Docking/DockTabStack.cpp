#include "Function/UI/Docking/DockTabStack.h"
#include "Function/UI/Docking/DockDragDropOperation.h"
#include "Function/UI/Docking/DockSpace.h"
#include "Function/UI/DrawElementList.h"
#include "Function/UI/Rect.h"

namespace Zafkiel
{

void DockTabStack::Construct()
{
    content
    .SetSizeRule(SizeRule::Grow)
    [
        panel = CreateWidget<Panel>()
        ->SetLayoutRule(LayoutRule::Vertical)
        ->AddChild(
            Panel::Slot()
            .SetSizeRule(SizeRule::Auto)
            .SetHorizontalAlignmentRule(HorizontalAlignmentRule::Fill)
            [
                CreateRef<Border>()
                ->SetBackground(
                    Border::BorderBackgroundSlot()
                    [
                        CreateWidget<Rect>(vec4(0.3f, 0.5f, 0.7f, 1.0f))
                    ]
                )
                ->SetContent(
                    SingleWidgetSlot()
                    .SetSizeRule(SizeRule::Grow)
                    [
                        dockTabBar = CreateRef<DockTabBar>()
                        ->SetParentStack(this)
                    ]
                )
            ]
        )
        ->AddChild(
            Panel::Slot()
            .SetSizeRule(SizeRule::Grow)
            .SetHorizontalAlignmentRule(HorizontalAlignmentRule::Fill)
        )
    ];
    tabContentSlotIndex = 1;
}

uint32 DockTabStack::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) 
{
    uint32 maxLayerId = panel->Draw(allocatedGeometry, drawElementList, widgetGeometryList, this, layerId);

    if (currentDockTargetDirection != DockTargetDirection::None)
    {
        vec2 targetQuadPosMin, targetQuadPosMax;
        if (currentDockTargetDirection == DockTargetDirection::Left)
        {
            targetQuadPosMin = allocatedGeometry.position;
            targetQuadPosMax = allocatedGeometry.position + vec2(allocatedGeometry.size.x * 0.5f, allocatedGeometry.size.y);
        }
        else if (currentDockTargetDirection == DockTargetDirection::Bottom)
        {
            targetQuadPosMin = allocatedGeometry.position + vec2(0, allocatedGeometry.size.y * 0.5f);
            targetQuadPosMax = allocatedGeometry.position + allocatedGeometry.size;
        }
        else if (currentDockTargetDirection == DockTargetDirection::Right)
        {
            targetQuadPosMin = allocatedGeometry.position + vec2(allocatedGeometry.size.x * 0.5f, 0);
            targetQuadPosMax = allocatedGeometry.position + allocatedGeometry.size;
        }
        else
        {
            targetQuadPosMin = allocatedGeometry.position;
            targetQuadPosMax = allocatedGeometry.position + vec2(allocatedGeometry.size.x, allocatedGeometry.size.y * 0.5f);
        }
        drawElementList.AddQuad(maxLayerId, targetQuadPosMin, targetQuadPosMax, vec4(1.0f, 1.0f, 1.0f, 0.3f));
    }

    return maxLayerId + 1;
}

void DockTabStack::ShowTabContent(Ref<Widget> tabContent)
{
    panel->GetChild(tabContentSlotIndex)
    [
        tabContent
    ];
}

void DockTabStack::OpenTab(const Ref<DockTab> &tab)
{
    dockTabBar->AddTab(tab);
    
}

void DockTabStack::OnLastTabRemoved()
{
    GetDockSpace()->CleanUp();
}

Reply DockTabStack::OnDragOver(WidgetGeometry widgetGeometry, DragDropEvent &event)
{
    auto dragDropOperation = event.GetOperation();
    if (dragDropOperation.Is<DockDragDropOperation>())
    {
        auto dockDragDropOperation = dragDropOperation.As<DockDragDropOperation>();

        currentDockTargetDirection = ComputeDockTargetDirection(widgetGeometry, event.GetPosition());

        return Reply::Handled();
    }

    return Reply::Unhandled();
}

Reply DockTabStack::OnDragLeave(WidgetGeometry widgetGeometry, DragDropEvent &event)
{
    auto dragDropOperation = event.GetOperation();
    if (dragDropOperation.Is<DockDragDropOperation>())
    {
        currentDockTargetDirection = DockTargetDirection::None;
        return Reply::Handled();
    }
    return Reply::Unhandled();
}

Reply DockTabStack::OnDrop(WidgetGeometry widgetGeometry, DragDropEvent &event)
{
    auto dragDropOperation = event.GetOperation();
    if (dragDropOperation.Is<DockDragDropOperation>())
    {
        auto dockDragDropOperation = dragDropOperation.As<DockDragDropOperation>();

        if (currentDockTargetDirection == DockTargetDirection::None)
        {
            return Reply::Unhandled();
        }

        auto newTabStack = CreateWidget<DockTabStack>();

        parent->PlaceNode(newTabStack, currentDockTargetDirection, this);

        newTabStack->OpenTab(dockDragDropOperation->GetDraggedTab());

        currentDockTargetDirection = DockTargetDirection::None;

        return Reply::Handled();
    }

    return Reply::Unhandled();
}

DockTabStack::DockTargetDirection DockTabStack::ComputeDockTargetDirection(WidgetGeometry widgetGeometry, vec2 position)
{
    vec2 dockTabBarSize = dockTabBar->GetDesiredSize();

    WidgetGeometry contentGeometry = WidgetGeometry {
        widgetGeometry.size - vec2(0, dockTabBarSize.y), 
        widgetGeometry.position + vec2(0, dockTabBarSize.y)
    };
    if (!contentGeometry.Contain(position))
    {
        return DockTargetDirection::None;
    }

    vec2 contentCenter = contentGeometry.Center();

    vec2 lt = contentGeometry.position;
    vec2 rt = contentGeometry.position + vec2(contentGeometry.size.x, 0);
    vec2 lb = contentGeometry.position + vec2(0, contentGeometry.size.y);
    vec2 rb = contentGeometry.position + contentGeometry.size;

    vec2 clt = lt - contentCenter;
    vec2 crt = rt - contentCenter;
    vec2 clb = lb - contentCenter;
    vec2 crb = rb - contentCenter;
    vec2 cpos = position - contentCenter;

    if (vec2::cross(clt, cpos) > 0 && vec2::cross(cpos, crt) > 0 &&
        position.y - widgetGeometry.position.y > 0 && position.y - widgetGeometry.position.y < widgetGeometry.size.y * 0.3f)
    {
        return DockTargetDirection::Top;
    }
    if (vec2::cross(clb, cpos) > 0 && vec2::cross(cpos, clt) > 0 &&
        position.x - widgetGeometry.position.x > 0 && position.x - widgetGeometry.position.x < widgetGeometry.size.x * 0.3f)
    {
        return DockTargetDirection::Left;
    }
    if (vec2::cross(crb, cpos) > 0 && vec2::cross(cpos, clb) > 0 &&
        position.y - widgetGeometry.position.y > widgetGeometry.size.y * 0.7f && position.y - widgetGeometry.position.y < widgetGeometry.size.y)
    {
        return DockTargetDirection::Bottom;
    }
    if (vec2::cross(crt, cpos) > 0 && vec2::cross(cpos, crb) > 0 &&
        position.x - widgetGeometry.position.x > widgetGeometry.size.x * 0.7f && position.x - widgetGeometry.position.x < widgetGeometry.size.x)
    {
        return DockTargetDirection::Right;
    }
    return DockTargetDirection::None;
}

}