#include "Function/UI/SubWindow.h"
#include "Function/UI/LayoutUtils.h"
#include "Function/UI/LayoutUtils.h"
#include "Function/UI/WidgetGeometryList.h"
#include "Platform/PlatformApplication.h"

namespace Zafkiel
{

CursorReply SubWindow::OnCursorQuery(WidgetGeometry widgetGeometry, PointerEvent &event)
{
    if (isResizing)
    {
        return CursorReply::Cursor(ResizeOrientationToCursorType(resizeOrientation));
    }
    if (PosInEdgeOrCorner(event.GetPosition(), resizeOrientation, resizeSide))
    {
        return CursorReply::Cursor(ResizeOrientationToCursorType(resizeOrientation));
    }
    return CursorReply::Unhandled();
}

Reply SubWindow::OnMouseButtonDown(WidgetGeometry widgetGeometry, PointerEvent &event)
{
    if (event.GetEffectingButton() == MouseButton::Left)
    {
        if (PosInEdgeOrCorner(event.GetPosition(), resizeOrientation, resizeSide))
        {
            isResizing = true;
            return Reply::Handled().CaptureMouse(this);
        }
        return Reply::Handled().CaptureMouse(this);
    }
    return Reply::Unhandled();
}

Reply SubWindow::OnMouseMove(WidgetGeometry widgetGeometry, PointerEvent& event) 
{
    if (isResizing && HasMouseCapture())
    {
        if (resizeSide == ResizeSide::BottomRight)
        {
            size = event.GetPosition() - position;
            return Reply::Handled();
        }
    }

    if (HasMouseCapture() && event.IsMouseButtonPressed(MouseButton::Left))
    {
        position = position + event.GetDeltaPosition();
        return Reply::Handled();
    }
    return Reply::Unhandled();
}

Reply SubWindow::OnMouseButtonUp(WidgetGeometry widgetGeometry, PointerEvent &event) 
{
    if (event.GetEffectingButton() == MouseButton::Left && isResizing)
    {
        isResizing = false;
        return Reply::Handled().ReleaseMouseCapture();
    }
    if (event.GetEffectingButton() == MouseButton::Left)
    {
        return Reply::Handled().ReleaseMouseCapture();
    }
    return Reply::Unhandled();
}

bool SubWindow::PosInEdgeOrCorner(vec2 inPos, ResizeOrientation &outOrientation, ResizeSide &outSize)
{
    if (vec2 bottomRightOffset = position + size - inPos; 
        bottomRightOffset.x < resizeDetectOffset && bottomRightOffset.y < resizeDetectOffset)
    {
        outOrientation = ResizeOrientation::SouthEast;
        outSize = ResizeSide::BottomRight;
        return true;
    }
    else if (vec2 topLeftOffset = inPos - position;
                topLeftOffset.x < resizeDetectOffset && topLeftOffset.y < resizeDetectOffset)
    {
        outOrientation = ResizeOrientation::SouthEast;
        outSize = ResizeSide::TopLeft;
        return true;
    }
    return false;
}

CursorType SubWindow::ResizeOrientationToCursorType(ResizeOrientation orientation)
{
    switch (orientation)
    {
        using enum ResizeOrientation;
    case LeftRight: return CursorType::ResizeLeftRight;
    case UpDown: return CursorType::ResizeUpDown;
    case SouthEast: return CursorType::ResizeSouthEast;
    case SouthWest: return CursorType::ResizeSouthWest;
    }
}

uint32 SubWindow::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) 
{
    WidgetGeometry selfGeometry 
    {
        .size = size,
        .position = position, 
    };

    std::vector<ArrangedWidget> arrangedChildren;
    ArrangeChildren(selfGeometry, arrangedChildren);

    persistentState.allocatedGeometry = selfGeometry;

    return content.widget->Draw(arrangedChildren[0].widgetGeometry, drawElementList, widgetGeometryList, this, layerId);
}

}
