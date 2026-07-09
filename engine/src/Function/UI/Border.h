#pragma once
#include "Function/UI/CompoundWidget.h"

namespace Zafkiel
{

class Border : public CompoundWidget
{
  public:
    class BorderBackgroundSlot : public WidgetSlotBase<BorderBackgroundSlot>
    {
    };

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

    Border *SetBackground(const BorderBackgroundSlot &background) 
    { 
        this->background = background; 
        return this;
    }
  
  private:
     BorderBackgroundSlot background;
};

}