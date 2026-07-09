#pragma once
#include "Function/UI/Slot.h"
#include "Function/UI/Widget.h"

namespace Zafkiel
{

class CompoundWidget : public Widget
{
  public:
    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

    virtual void ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const override;

    virtual vec2 GetDesiredSize() const override;

    CompoundWidget *SetContent(const SingleWidgetSlot &slot)
    {
        this->content = slot;
        return this;
    }

  protected:
    SingleWidgetSlot content;

};

}
