#include "Function/UI/LayoutUtils.h"

namespace Zafkiel
{

namespace LayoutUtils
{

ArrangedWidget ArrangeSingleChild(SingleWidgetSlot child, WidgetGeometry allocatedGeometry)
{
    WidgetGeometry actualGeometry = WidgetGeometry {
        .size = allocatedGeometry.size - vec2(child.padding.left + child.padding.right, child.padding.top + child.padding.bottom),
        .position = allocatedGeometry.position + vec2(child.padding.left, child.padding.top),
    };

    float fixedSize = 0;
    float totalGrowFactor = 0;

    vec2 childDesiredSize = child.widget->GetDesiredSize();

    if (child.sizeRule == SizeRule::Auto)
    {
        vec2 arrangedPosition, arrangedSize;
        if (child.horizontalAlignmentRule == HorizontalAlignmentRule::Fill)
        {
            arrangedPosition.x = actualGeometry.position.x;
            arrangedSize.x = actualGeometry.size.x;
        }
        else if (child.horizontalAlignmentRule == HorizontalAlignmentRule::Left)
        {
            arrangedPosition.x = actualGeometry.position.x;
            arrangedSize.x = childDesiredSize.x;
        }
        else if (child.horizontalAlignmentRule == HorizontalAlignmentRule::Center)
        {
            arrangedPosition.x = actualGeometry.position.x + std::max(0.0f, (actualGeometry.size.x - childDesiredSize.x)) * 0.5f;
            arrangedSize.x = childDesiredSize.x;
        }
        else
        {
            arrangedPosition.x = actualGeometry.position.x + std::max(0.0f, (actualGeometry.size.x - childDesiredSize.x));
            arrangedSize.x = childDesiredSize.x;
        }

        if (child.verticalAlignmentRule == VerticalAlignmentRule::Fill)
        {
            arrangedPosition.y = actualGeometry.position.y;
            arrangedSize.y = actualGeometry.size.y;
        }
        else if (child.verticalAlignmentRule == VerticalAlignmentRule::Top)
        {
            arrangedPosition.y = actualGeometry.position.y;
            arrangedSize.y = childDesiredSize.y;
        }
        else if (child.verticalAlignmentRule == VerticalAlignmentRule::Center)
        {
            arrangedPosition.y = actualGeometry.position.y + std::max(0.0f, (actualGeometry.size.y - childDesiredSize.y)) * 0.5f;
            arrangedSize.y = childDesiredSize.y;
        }
        else
        {
            arrangedPosition.y = actualGeometry.position.y + std::max(0.0f, (actualGeometry.size.y - childDesiredSize.y));
            arrangedSize.y = childDesiredSize.y;
        }
        return ArrangedWidget {
            .widget = child.widget.get(),
            .widgetGeometry = WidgetGeometry {arrangedSize, arrangedPosition}
        };
    }
    else // child.sizeRule == SizeRule::Grow
    {
        return ArrangedWidget {
            .widget = child.widget.get(),
            .widgetGeometry = actualGeometry,
        };
    }
}

}

}