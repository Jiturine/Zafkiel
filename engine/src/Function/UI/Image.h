#pragma once
#include "Function/UI/Widget.h"
#include "Function/RHI/RHIResources.h" // TODO: 不应该向UI暴露RHI层

namespace Zafkiel 
{

class Image : public Widget
{
  public:
    virtual vec2 GetDesiredSize() const override;

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

  private:
    Ref<RHITexture> texture;

    vec4 color;
};

}