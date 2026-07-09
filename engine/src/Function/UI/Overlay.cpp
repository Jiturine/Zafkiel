#include "Function/UI/Overlay.h"
#include "Function/UI/LayoutUtils.h"

namespace Zafkiel
{

vec2 Overlay::GetDesiredSize() const 
{
    vec2 maxSize(0.0f, 0.0f);
    for (auto &child : children)
    {
        vec2 childDesiredSize = child.widget->GetDesiredSize();
        maxSize.x = std::max(maxSize.x, childDesiredSize.x);
        maxSize.y = std::max(maxSize.y, childDesiredSize.y);
    }
    return maxSize;
}

void Overlay::ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const
{
    for (auto &child : children)
    {
        arrangedChildren.push_back(LayoutUtils::ArrangeSingleChild(child, allocatedGeometry));
    }
}

uint32 Overlay::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId)
{
    std::vector<ArrangedWidget> arrangedChildren;
    ArrangeChildren(allocatedGeometry, arrangedChildren);

    uint32 maxLayerId = layerId;

    int curChildIndex = 0;
    for (auto &arrangedChild : arrangedChildren)
    {
        maxLayerId = std::max(maxLayerId, 
            arrangedChild.widget->Draw(allocatedGeometry, drawElementList, widgetGeometryList, this, maxLayerId + ((curChildIndex != 0) ? 1 : 0)));
        curChildIndex++;
    }
    return maxLayerId;
}

}