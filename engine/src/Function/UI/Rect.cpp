#include "Function/UI/Rect.h"
#include "Function/UI/DrawElementList.h"
#include "Function/UI/WidgetGeometryList.h"

namespace Zafkiel
{

uint32 Rect::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId)
{
    drawElementList.AddQuad(layerId, allocatedGeometry.position, allocatedGeometry.position + allocatedGeometry.size, color);

    widgetGeometryList.AddWidgetGeometry(this, allocatedGeometry, layerId);

    return layerId;
}
    
}