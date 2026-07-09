#include "Function/UI/Panel.h"
#include "Function/UI/LayoutUtils.h"

namespace Zafkiel 
{

vec2 Panel::GetDesiredSize() const
{
    if (size != vec2(0.0f, 0.0f)) // Panel 如有固定size的话
        return size;

    vec2 desiredSize(0.0f, 0.0f);

    for (auto [i, child] : std::views::enumerate(children))
    {
        auto childSize = child.widget->GetDesiredSize();
        if (layoutRule == LayoutRule::Vertical)
        {
            if (i > 0) desiredSize.y += space.vertical;

            desiredSize.y += childSize.y;
            desiredSize.x = std::max(desiredSize.x, childSize.x);
        }
        else 
        {
            if (i > 0) desiredSize.x += space.horizontal;

            desiredSize.x += childSize.x;
            desiredSize.y = std::max(desiredSize.y, childSize.y);
        }
    }

    desiredSize = desiredSize + vec2(padding.left + padding.right, padding.top + padding.bottom);

    return desiredSize;
}

void Panel::ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const
{
    vec2 actualSize = allocatedGeometry.size - vec2(padding.left + padding.right, padding.top + padding.bottom);
    vec2 actualPosition = allocatedGeometry.position + vec2(padding.left, padding.top);
    float axisSpace = ((layoutRule == LayoutRule::Horizontal) ? space.horizontal : space.vertical);

    float totalSize = (layoutRule == LayoutRule::Horizontal) ? actualSize.x : actualSize.y;

    float fixedSize = 0;
    float totalGrowFactor = 0;

    for (auto &child : children)
    {
        if (child.sizeRule == SizeRule::Auto)
        {
            vec2 childDesiredSize = child.widget->GetDesiredSize();
            fixedSize += (layoutRule == Horizontal) ? childDesiredSize.x : childDesiredSize.y;
        }
        else if (child.sizeRule == SizeRule::Grow)
        {
            totalGrowFactor += child.growFactor;
        }
    }
    if (!children.empty())
    {
        fixedSize += (children.size() - 1) * axisSpace;
    }

    // 分配空间
    float availavleSize = std::max(0.0f, totalSize - fixedSize);
    float offset = 0;

    arrangedChildren.clear();
    for (auto &child : children)
    {
        vec2 childDesiredSize = child.widget->GetDesiredSize();

        float alongAxisSize;
        if (child.sizeRule == SizeRule::Auto)
        {
            alongAxisSize = (layoutRule == Horizontal) ? childDesiredSize.x : childDesiredSize.y;
        }
        else if (child.sizeRule == SizeRule::Grow)
        {
            // 按比例分配可用空间
            float ratio = child.growFactor / totalGrowFactor;
            alongAxisSize = availavleSize * ratio;
        }

        // 应用 Min/Max 限制
        // ChildSize = FMath::Clamp(ChildSize,
        //     CurChild.GetMinSize().Get(),
        //     CurChild.GetMaxSize().Get() > 0 ? CurChild.GetMaxSize().Get() : ChildSize);

        float crossAxisSize;
        float alongAxisPosition;
        float crossAxisPosition;
        if (layoutRule == LayoutRule::Horizontal)
        {
            if (child.verticalAlignmentRule == VerticalAlignmentRule::Fill)
            {
                crossAxisSize = actualSize.y;
            }
            else
            {
                crossAxisSize = std::min(childDesiredSize.y, actualSize.y);
            }
            alongAxisPosition = actualPosition.x + offset;
            if (child.verticalAlignmentRule == VerticalAlignmentRule::Fill || child.verticalAlignmentRule == VerticalAlignmentRule::Top)
            {
                crossAxisPosition = actualPosition.y;
            }
            else if (child.verticalAlignmentRule == VerticalAlignmentRule::Center)
            {
                crossAxisPosition = actualPosition.y + (actualSize.y - childDesiredSize.y) / 2.0f;
            }
            else
            {
                crossAxisPosition = actualPosition.y + actualSize.y - childDesiredSize.y;
            }
        }
        else
        {

            if (child.horizontalAlignmentRule == HorizontalAlignmentRule::Fill)
            {
                crossAxisSize = actualSize.x;
            }
            else
            {
                crossAxisSize = std::min(childDesiredSize.x, actualSize.x);
            }
            alongAxisPosition = actualPosition.y + offset;
            if (child.horizontalAlignmentRule == HorizontalAlignmentRule::Fill || child.horizontalAlignmentRule == HorizontalAlignmentRule::Left)
            {
                crossAxisPosition = actualPosition.x;
            }
            else if (child.horizontalAlignmentRule == HorizontalAlignmentRule::Center)
            {
                crossAxisPosition = actualPosition.x + (actualSize.x - childDesiredSize.x) / 2.0f;
            }
            else
            {
                crossAxisPosition = actualPosition.x + actualSize.x - childDesiredSize.x;
            }
        }

        vec2 arrangedSize = (layoutRule == LayoutRule::Horizontal) ? vec2(alongAxisSize, crossAxisSize) 
                                                                   : vec2(crossAxisSize, alongAxisSize);

        vec2 arrangedPosition = (layoutRule == LayoutRule::Horizontal) ? vec2(alongAxisPosition, crossAxisPosition)
                                                                       : vec2(crossAxisPosition, alongAxisPosition);
        
        arrangedChildren.emplace_back(child.widget.get(), WidgetGeometry {arrangedSize, arrangedPosition});

        offset += alongAxisSize + axisSpace;
    }
}

uint32 Panel::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) 
{
    std::vector<ArrangedWidget> arrangedChildren;
    ArrangeChildren(allocatedGeometry, arrangedChildren);

    uint32 maxLayerId = layerId;
    for (auto &child : arrangedChildren)
    {
        maxLayerId = std::max(maxLayerId, child.widget->Draw(child.widgetGeometry, drawElementList, widgetGeometryList, this, layerId));
    }
    return maxLayerId;
}

}