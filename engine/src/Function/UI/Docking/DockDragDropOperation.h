#pragma once
#include "Function/UI/Docking/DockSubWindow.h"
#include "Function/UI/InputEvent.h"
#include "Function/UI/Docking/DockTab.h"
#include "Function/UI/SubWindow.h"

namespace Zafkiel
{
class DockSpace;
class DockTabStack;

class DockDragDropOperation : public DragDropOperation
{
  public:
    DockDragDropOperation(Ref<DockTab> inDraggedTab, vec2 inTabGrabOffsetFraction, vec2 tabStackSize);

    Ref<DockTab> GetDraggedTab() const
    {
        return draggedTab;
    }

    vec2 GetTabGrabOffsetFraction() const
    {
        return tabGrabOffsetFraction;
    }

    void FloatTab(const DragDropEvent &dragDropEvent);

    void PinTab();

    void DroppedOntoNothing();

    virtual void OnDragged(const DragDropEvent &dragDropEvent) override;

    virtual void OnDrop(bool handled, const DragDropEvent &dragDropEvent) override;

  private:
    vec2 GetFloatingWindowOffsetFromCursor();

    Ref<DockTab> draggedTab;

    vec2 tabGrabOffsetFraction;

    vec2 tabStackSize;

    bool tabIsFloating = false;

    WeakRef<DockSpace> rootDockSpace;

    Ref<DockSubWindow> floatingWindow;

    Ref<DockTabStack> floatingDockTabStack;
};

}