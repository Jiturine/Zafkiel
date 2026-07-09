#include "Function/UI/Docking/DockSpace.h"
#include "Function/UI/Docking/DockDragDropOperation.h"
#include "Function/UI/Docking/DockTabStack.h"
#include "Function/UI/DrawElementList.h"
#include "Function/UI/WidgetGeometryList.h"

namespace Zafkiel
{

uint32 DockSpace::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) 
{
    widgetGeometryList.AddWidgetGeometry(this, allocatedGeometry, layerId);
    
    uint32 maxLayerId = DockSplitter::GenerateDrawElements(allocatedGeometry, drawElementList, widgetGeometryList, layerId);

    if (children.size() != 0)
    {
        currentDockTargetDirection = DockTargetDirection::None;
    }

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
    maxLayerId++;

    for (auto &subWindow : subWindows)
    {
        maxLayerId = std::max(maxLayerId, subWindow->Draw({}, drawElementList, widgetGeometryList, persistentState.paintParent.Lock(), maxLayerId + 1));
    }

    return maxLayerId;
}

DockSpace::DockTargetDirection DockSpace::ComputeDockTargetDirection(WidgetGeometry widgetGeometry, vec2 position)
{
    vec2 contentCenter = widgetGeometry.Center();

    vec2 lt = widgetGeometry.position;
    vec2 rt = widgetGeometry.position + vec2(widgetGeometry.size.x, 0);
    vec2 lb = widgetGeometry.position + vec2(0, widgetGeometry.size.y);
    vec2 rb = widgetGeometry.position + widgetGeometry.size;

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

Reply DockSpace::OnDragOver(WidgetGeometry widgetGeometry, DragDropEvent &event) 
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

Reply DockSpace::OnDragLeave(WidgetGeometry widgetGeometry, DragDropEvent &event)
{
    auto dragDropOperation = event.GetOperation();
    if (dragDropOperation.Is<DockDragDropOperation>())
    {
        currentDockTargetDirection = DockTargetDirection::None;
        return Reply::Handled();
    }
    return Reply::Unhandled();
}

Reply DockSpace::OnDrop(WidgetGeometry widgetGeometry, DragDropEvent &event)
{
    auto dragDropOperation = event.GetOperation();
    if (dragDropOperation.Is<DragDropOperation>())
    {
        auto dockDragDropOperation = dragDropOperation.As<DockDragDropOperation>();

        if (currentDockTargetDirection == DockTargetDirection::None)
        {
            return Reply::Unhandled();
        }

        assert(children.empty());
        // if (children.size() > 1 && !DirectionMatchLayoutRule(currentDockTargetDirection))
        // {
        //     Ref<DockSplitter> newSplitter = CreateWidget<DockSplitter>(splitter->GetLayoutRule());
        //     for (auto [index, child] : std::views::enumerate(children))
        //     {
        //         auto sizeFactor = splitter->GetChild(index).sizeFactor;
        //         newSplitter->AddChild(child, sizeFactor);
        //     }
        //     while (children.size() > 0) 
        //     {
        //         RemoveChildAt(children.size() - 1);
        //     }
        //     AddChild(newSplitter);

        //     LayoutRule newLayoutRule = splitter->GetLayoutRule() == LayoutRule::Horizontal ? LayoutRule::Vertical : LayoutRule::Horizontal;
        //     splitter->SetLayoutRule(newLayoutRule);
        // }

        auto newTabStack = CreateWidget<DockTabStack>();
        
        newTabStack->OpenTab(dockDragDropOperation->GetDraggedTab());

        if (currentDockTargetDirection == DockTargetDirection::Left || currentDockTargetDirection == DockTargetDirection::Top)
        {
            InsertChild(newTabStack, 0);
        }
        else
        {
            AddChild(newTabStack);
        }

        currentDockTargetDirection = DockTargetDirection::None;

        return Reply::Handled();
    }

    return Reply::Unhandled();
}

void DockSpace::CleanUp()
{
    auto cleanUpResult = CleanUpNodes();

    if (cleanUpResult == CleanUpResult::NoTabsUnderNode)
    {
        if (parentSubWindow.IsValid())
        {
            auto subWindow = parentSubWindow.Lock();
            subWindow->GetParentDockSpace()->RemoveSubWindow(subWindow);
        }
    }
}

}