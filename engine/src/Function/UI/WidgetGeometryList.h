#pragma once
#include "Function/UI/Widget.h"

namespace Zafkiel
{

class WidgetGeometryList
{
  public:
    struct WidgetGeometryElement
    {
        Ref<Widget> widget;

        WidgetGeometry widgetGeometry;
        
        uint32 layerId;
    };

    void AddWidgetGeometry(Widget *widget, WidgetGeometry widgetGeometry, uint32 layerId)
    {
        Ref<Widget> ancestor = widget->GetPaintParent();
        while (ancestor)
        {
            if (!ancestor->GetVisibility().AreChildrenHitTestVisible())
                return;
            ancestor = ancestor->GetPaintParent();
        }
        elements.emplace_back(widget, widgetGeometry, layerId);
    }

    void Sort()
    {
        std::stable_sort(elements.begin(), elements.end(), [](const WidgetGeometryElement &a, const WidgetGeometryElement &b) {
            return a.layerId < b.layerId;
        });
    }

    void Clear() { elements.clear(); }

    std::vector<WidgetGeometryElement> &GetElements() { return elements; }

  private:
    std::vector<WidgetGeometryElement> elements;
};

}