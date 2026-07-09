#pragma once
#include "Function/UI/Widget.h"

namespace Zafkiel
{

class Rect : public Widget
{
  public:
    void Construct(vec4 color, vec2 size = vec2(0.0f))
    {
        this->color = color;
        this->size = size;
    }

    virtual vec2 GetDesiredSize() const override { return size; }

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

  private:
    vec2 size = vec2(0.0f);

    vec4 color;
};

}