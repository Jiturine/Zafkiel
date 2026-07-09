#pragma once
#include "Function/UI/Widget.h"
#include "Function/UI/Slot.h"

namespace Zafkiel
{

class Overlay : public Widget
{
  public:
    virtual vec2 GetDesiredSize() const override;

    virtual void ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const override;

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

  private:
    std::vector<SingleWidgetSlot> children;
};

}