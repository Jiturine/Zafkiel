#include "Function/UI/Border.h"
#include "Function/UI/LayoutUtils.h"

namespace Zafkiel
{

uint32 Border::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId)
{
    uint32 backgroundMaxLayerId = background.widget->Draw(allocatedGeometry, drawElementList, widgetGeometryList, this, layerId);

    return CompoundWidget::GenerateDrawElements(allocatedGeometry, drawElementList, widgetGeometryList, backgroundMaxLayerId + 1);
}
    
}