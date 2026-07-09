#include "Function/UI/Docking/DockDragDropOperation.h"
#include "Function/UI/Docking/DockSpace.h"
#include "Function/UI/Docking/DockTabStack.h"

namespace Zafkiel
{

DockDragDropOperation::DockDragDropOperation(Ref<DockTab> inDraggedTab, vec2 inTabGrabOffsetFraction, vec2 tabStackSize)
    : draggedTab(inDraggedTab), tabGrabOffsetFraction(inTabGrabOffsetFraction), tabStackSize(tabStackSize)
{
    rootDockSpace = inDraggedTab->GetParent()->GetParentStack()->GetDockSpace();
    while (rootDockSpace->GetParentSubWindow())
    {
        rootDockSpace = rootDockSpace->GetParentSubWindow()->GetParentDockSpace();
    }
}

void DockDragDropOperation::OnDragged(const DragDropEvent &dragDropEvent) 
{
    if (tabIsFloating)
    {
        floatingWindow->MoveTo(dragDropEvent.GetPosition() - GetFloatingWindowOffsetFromCursor());
    }
}

void DockDragDropOperation::OnDrop(bool handled, const DragDropEvent &dragDropEvent)
{
    if (!handled)
    {
        DroppedOntoNothing();
    }
    else if (tabIsFloating)
    {
        PinTab();
    }
    draggedTab->SetVisibility(Visibility::Visible);

    draggedTab = nullptr;
}

void DockDragDropOperation::FloatTab(const DragDropEvent &dragDropEvent)
{
    tabIsFloating = true;

    floatingDockTabStack = CreateWidget<DockTabStack>();
    floatingDockTabStack->OpenTab(draggedTab);

    auto dockSpace = CreateWidget<DockSpace>();
    dockSpace->AddChild(floatingDockTabStack);

    vec2 subWindowPos = dragDropEvent.GetPosition() - GetFloatingWindowOffsetFromCursor();
    floatingWindow = CreateWidget<DockSubWindow>(subWindowPos, tabStackSize);
    floatingWindow->SetContent(SingleWidgetSlot().SetSizeRule(SizeRule::Grow)[dockSpace]);
    floatingWindow->SetVisibility(Visibility::HitTestInvisible);

    dockSpace->SetParentSubWindow(floatingWindow.get());

    rootDockSpace->AddSubWindow(floatingWindow);
}

void DockDragDropOperation::PinTab()
{
    if (tabIsFloating && floatingDockTabStack)
    {
        tabIsFloating = false;

        floatingDockTabStack->GetDockTabBar()->RemoveTab(draggedTab);

        floatingDockTabStack->OnLastTabRemoved();

        floatingDockTabStack = nullptr;
        floatingWindow = nullptr;
    }
}

vec2 DockDragDropOperation::GetFloatingWindowOffsetFromCursor()
{
    vec2 floatingTabSize = vec2(tabStackSize.x, draggedTab->GetDesiredSize().y);
    return tabGrabOffsetFraction * floatingTabSize;
}

void DockDragDropOperation::DroppedOntoNothing()
{
    floatingWindow->SetVisibility(Visibility::Visible);
}
    
}