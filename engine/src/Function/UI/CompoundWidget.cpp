#include "Function/UI/CompoundWidget.h"
#include "Function/UI/LayoutUtils.h"

namespace Zafkiel
{

uint32 CompoundWidget::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) 
{
    if (!content.IsValid())
        return layerId;
    
    std::vector<ArrangedWidget> arrangedChildren;
    ArrangeChildren(allocatedGeometry, arrangedChildren);

    return content.widget->Draw(arrangedChildren[0].widgetGeometry, drawElementList, widgetGeometryList, this, layerId + 1);
}

void CompoundWidget::ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const 
{
    arrangedChildren.push_back(LayoutUtils::ArrangeSingleChild(content, allocatedGeometry));
}

vec2 CompoundWidget::GetDesiredSize() const 
{
    return content.widget->GetDesiredSize() + vec2(content.padding.left + content.padding.right, content.padding.top + content.padding.bottom);
}
    
}