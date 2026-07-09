#pragma once

#include "Function/UI/Docking/DockSubWindow.h"
#include "Function/UI/SubWindow.h"
#include "Function/UI/Docking/DockSplitter.h"

namespace Zafkiel
{

class DockSpace : public DockSplitter
{
  public:
    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

    virtual Reply OnDragOver(WidgetGeometry widgetGeometry, DragDropEvent &event) override;

    virtual Reply OnDragLeave(WidgetGeometry widgetGeometry, DragDropEvent &event) override;

    virtual Reply OnDrop(WidgetGeometry widgetGeometry, DragDropEvent &event) override;
  
    virtual Ref<DockSpace> GetDockSpace() override { return this; }

    void CleanUp();

    void Construct() 
    {
        DockSplitter::Construct(LayoutRule::Horizontal);
    }

    void SetParentSubWindow(Ref<DockSubWindow> subWindow)
    {
        parentSubWindow = subWindow;
    }

    Ref<DockSubWindow> GetParentSubWindow() const
    {
        return parentSubWindow.Lock();
    }

    void AddSubWindow(const Ref<DockSubWindow> &subWindow)
    {
        subWindows.push_back(subWindow);
        subWindow->SetParentDockSpace(this);
    }

    void RemoveSubWindow(const Ref<DockSubWindow> &subWindow)
    {
        subWindows.erase(std::find(subWindows.begin(), subWindows.end(), subWindow));
    }

    virtual Type GetNodeType() const override { return Type::DockSpace; }
    
  private:
    DockTargetDirection ComputeDockTargetDirection(WidgetGeometry widgetGeometry, vec2 position);

    DockTargetDirection currentDockTargetDirection = DockTargetDirection::None;
  
    std::vector<Ref<DockSubWindow>> subWindows;

    WeakRef<DockSubWindow> parentSubWindow;
};

}