#include "Function/UI/Image.h"
#include "Function/UI/DrawElementList.h"
#include "Function/UI/WidgetGeometryList.h"

namespace Zafkiel 
{

vec2 Image::GetDesiredSize() const
{
    return vec2(texture->GetWidth(), texture->GetHeight());
}

uint32 Image::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId)
{
    drawElementList.AddQuad(layerId, allocatedGeometry.position, allocatedGeometry.position + allocatedGeometry.size, color, texture.get());

    widgetGeometryList.AddWidgetGeometry(this, allocatedGeometry, layerId);

    return layerId;
}

}