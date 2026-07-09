#pragma once
#include "Function/UI/CompoundWidget.h"

namespace Zafkiel
{

class SubWindow : public CompoundWidget
{
  public:
    void Construct(vec2 position, vec2 size) 
    {
        this->position = position;
        this->size = size;
    }

    enum class ResizeOrientation
    {
        LeftRight,
        UpDown,
        SouthEast,
        SouthWest,
    };

    enum class ResizeSide
    {
        Top, Bottom, Left, Right,
        TopLeft, TopRight,
        BottomLeft, BottomRight,
    };

    vec2 GetDesiredSize() const override { return size; }

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

    virtual CursorReply OnCursorQuery(WidgetGeometry widgetGeometry, PointerEvent &event) override;

    virtual Reply OnMouseButtonDown(WidgetGeometry widgetGeometry, PointerEvent &event) override;

    virtual Reply OnMouseMove(WidgetGeometry widgetGeometry, PointerEvent& event) override;

    virtual Reply OnMouseButtonUp(WidgetGeometry widgetGeometry, PointerEvent &event) override;

    void MoveTo(vec2 position)
    {
        this->position = position;
    }

  private:
    bool PosInEdgeOrCorner(vec2 inPos, ResizeOrientation &outOrientation, ResizeSide &outSize);

    CursorType ResizeOrientationToCursorType(ResizeOrientation orientation);

    vec2 size;

    vec2 position;

    bool isResizing;

    ResizeOrientation resizeOrientation;

    ResizeSide resizeSide;

    static constexpr float resizeDetectOffset = 5.0f;
};

}
