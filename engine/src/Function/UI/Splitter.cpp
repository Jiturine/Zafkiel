#include "Function/UI/Splitter.h"
#include "Function/UI/DrawElementList.h"

namespace Zafkiel
{

vec2 Splitter::GetDesiredSize() const
{
    vec2 desiredSize(0.0f, 0.0f);

    for (auto [i, child] : std::views::enumerate(children))
    {
        auto childSize = child.widget->GetDesiredSize();
        if (layoutRule == LayoutRule::Vertical)
        {
            desiredSize.y += childSize.y;
            desiredSize.x = std::max(desiredSize.x, childSize.x);
        }
        else 
        {
            desiredSize.x += childSize.x;
            desiredSize.y = std::max(desiredSize.y, childSize.y);
        }
    }

    return desiredSize;
}

void Splitter::ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const 
{
    float totalFactor = 0.0f;

    for (auto &child : children)
    {
        totalFactor += child.sizeFactor;
    }

    float offset = 0;
    float alongAxisTotalSize = (layoutRule == LayoutRule::Horizontal) ? allocatedGeometry.size.x : allocatedGeometry.size.y;
    float totalHandleSize = std::max(0ul, children.size() - 1) * splitterHandleSize;
    float resizableSize = alongAxisTotalSize - totalHandleSize;

    for (auto &child : children)
    {
        float alongAxisSize = child.sizeFactor / totalFactor * resizableSize;

        vec2 arrangedSize = (layoutRule == LayoutRule::Horizontal) ? vec2(alongAxisSize, allocatedGeometry.size.y)
                                                                   : vec2(allocatedGeometry.size.x, alongAxisSize);

        vec2 arrangedPosition = (layoutRule == LayoutRule::Horizontal) ? vec2(allocatedGeometry.position.x + offset, allocatedGeometry.position.y)
                                                                       : vec2(allocatedGeometry.position.x, allocatedGeometry.position.y + offset);

        arrangedChildren.emplace_back(child.widget.get(), WidgetGeometry{arrangedSize, arrangedPosition});

        offset += alongAxisSize + splitterHandleSize;
    }
}

uint32 Splitter::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) 
{
    std::vector<ArrangedWidget> arrangedChildren;
    ArrangeChildren(allocatedGeometry, arrangedChildren);
    
    uint32 maxLayerId = layerId;
    for (auto &child : arrangedChildren)
    {
        maxLayerId = std::max(maxLayerId, child.widget->Draw(child.widgetGeometry, drawElementList, widgetGeometryList, this, layerId));
    }

    // 绘制 splitter 线
    uint32 splitterLayerId = maxLayerId + 1;

    for (uint32 splitterIndex = 1; splitterIndex < arrangedChildren.size(); splitterIndex++)
    {
        auto &arrangedChild = arrangedChildren[splitterIndex];
        vec2 handlePosition = arrangedChild.widgetGeometry.position - ((layoutRule == LayoutRule::Horizontal) ? 
                              vec2(splitterHandleSize, 0) : vec2(0, splitterHandleSize));
        vec2 handleSize = (layoutRule == LayoutRule::Horizontal) ? vec2(splitterHandleSize, arrangedChild.widgetGeometry.size.y)
                                                                 : vec2(arrangedChild.widgetGeometry.size.x, splitterHandleSize);
        drawElementList.AddQuad(splitterLayerId, handlePosition, handlePosition + handleSize, vec4(0.1f, 1.0f, 0.1f, 1.0f));
    }
    return splitterLayerId;
}

CursorReply Splitter::OnCursorQuery(WidgetGeometry widgetGeometry, PointerEvent &event) 
{
    if (isResizing)
    {
        if (layoutRule == LayoutRule::Horizontal)
            return CursorReply::Cursor(CursorType::ResizeLeftRight);
        else
            return CursorReply::Cursor(CursorType::ResizeUpDown);
    }

    std::vector<ArrangedWidget> arrangedChildren;
    ArrangeChildren(widgetGeometry, arrangedChildren);

    hoveredHandleIndex = GetHandleIndexUnderMouse(event.GetPosition(), arrangedChildren);
    if (hoveredHandleIndex != INDEX_NONE)
    {
        if (layoutRule == LayoutRule::Horizontal)
            return CursorReply::Cursor(CursorType::ResizeLeftRight);
        else
            return CursorReply::Cursor(CursorType::ResizeUpDown);
    }
    return CursorReply::Unhandled();
}

int32 Splitter::GetHandleIndexUnderMouse(vec2 mousePosition, const std::vector<ArrangedWidget> &arrangedChildren)
{
    for (int childIndex = 1; childIndex < arrangedChildren.size(); childIndex++)
    {
        if (layoutRule == LayoutRule::Horizontal)
        {
            float handleCenter = arrangedChildren[childIndex].widgetGeometry.position.x - 0.5f * splitterHandleSize;
            float leftBound = handleCenter - 0.5f * hitDetectionHandleSize;
            float rightBound = handleCenter + 0.5f * hitDetectionHandleSize;

            if (mousePosition.x > leftBound && mousePosition.x < rightBound)
                return childIndex - 1;
        }
        else
        {
            float handleCenter = arrangedChildren[childIndex].widgetGeometry.position.y - 0.5f * splitterHandleSize;
            float topBound = handleCenter - 0.5f * hitDetectionHandleSize;
            float bottomBound = handleCenter + 0.5f * hitDetectionHandleSize;

            if (mousePosition.y > topBound && mousePosition.y < bottomBound)
                return childIndex - 1;
        }
    }
    return INDEX_NONE;
}

Reply Splitter::OnMouseButtonDown(WidgetGeometry widgetGeometry, PointerEvent &event)
{
    if (event.GetEffectingButton() == MouseButton::Left)
    {
        if (hoveredHandleIndex != INDEX_NONE)
        {
            isResizing = true;
            return Reply::Handled().CaptureMouse(this);
        }
    }
    return Reply::Unhandled();
}

Reply Splitter::OnMouseMove(WidgetGeometry widgetGeometry, PointerEvent& event) 
{
    if (isResizing && HasMouseCapture())
    {
        HandleResizing(widgetGeometry, event.GetPosition());
    }
    return Reply::Unhandled();
}

Reply Splitter::OnMouseButtonUp(WidgetGeometry widgetGeometry, PointerEvent &event) 
{
    if (event.GetEffectingButton() == MouseButton::Left && isResizing)
    {
        isResizing = false;
        return Reply::Handled().ReleaseMouseCapture();
    }
    return Reply::Unhandled();
}

void Splitter::HandleResizing(WidgetGeometry widgetGeometry, vec2 mousePosition)
{
    std::vector<ArrangedWidget> arrangedChildren;
    ArrangeChildren(widgetGeometry, arrangedChildren);

    float mousePosAlongAxis = GetAlongAxis(mousePosition);
    float handleCenterPosAlongAxis = GetAlongAxis(arrangedChildren[hoveredHandleIndex + 1].widgetGeometry.position) - 0.5f * splitterHandleSize;
    float delta = mousePosAlongAxis - handleCenterPosAlongAxis;

    int beforeHandleIndex = hoveredHandleIndex;
    int afterHandleIndex = hoveredHandleIndex + 1;

    float slotBeforeOldSize = GetAlongAxis(arrangedChildren[beforeHandleIndex].widgetGeometry.size);
    float slotAfterOldSize = GetAlongAxis(arrangedChildren[afterHandleIndex].widgetGeometry.size);

    float slotBeforeNewSize = slotBeforeOldSize + delta;
    float slotAfterNewSize = slotAfterOldSize - delta;

    float totalSize = slotBeforeNewSize + slotAfterNewSize;
    float totalSizeFactor = children[beforeHandleIndex].sizeFactor + children[afterHandleIndex].sizeFactor;

    children[beforeHandleIndex].sizeFactor = totalSizeFactor * (slotBeforeNewSize / totalSize);
    children[afterHandleIndex].sizeFactor = totalSizeFactor * (slotAfterNewSize / totalSize);
}

}