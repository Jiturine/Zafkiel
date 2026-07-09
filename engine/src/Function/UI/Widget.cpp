#include "Function/UI/Widget.h"
#include "Function/UI/UISystem.h"

namespace Zafkiel
{

uint32 Widget::Draw(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, Ref<Widget> parent, uint32 layerId)
{
    persistentState.paintParent = parent;
    persistentState.allocatedGeometry = allocatedGeometry;

    uint32 maxLayerId = GenerateDrawElements(allocatedGeometry, drawElementList, widgetGeometryList, layerId);

    return maxLayerId;
}

bool Widget::HasMouseCapture()
{
    return UISystem::Instance().WidgetHasMouseCapture(this);
}

WidgetPath WeakWidgetPath::ToWidgetPath()
{
    WidgetPath widgetPath;
    for (auto &widget : std::views::reverse(widgets))
    {
        if (widget.IsValid())
        {
            widgetPath.Insert(ArrangedWidget {widget.Lock(), widget->GetPaintGeometry()}, 0);
        }
        else break;
    }
    return widgetPath;
}
    
}