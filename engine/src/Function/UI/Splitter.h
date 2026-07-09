#pragma once
#include "Function/UI/Widget.h"
#include "Function/UI/Slot.h"
#include "Function/UI/LayoutUtils.h"

namespace Zafkiel 
{

class Splitter : public Widget
{
  public:
    class Slot : public WidgetSlotBase<Slot>
    {
      public:
        float sizeFactor = 1.0f;

        Slot &SetSizeFactor(float sizeFactor)
        {
            this->sizeFactor = sizeFactor;
            return Self();
        }
    };

    void Construct(LayoutRule layoutRule)
    {
        this->layoutRule = layoutRule;
    }

    virtual vec2 GetDesiredSize() const override;

    virtual void ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const override;

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

    virtual CursorReply OnCursorQuery(WidgetGeometry widgetGeometry, PointerEvent &event) override;

    virtual Reply OnMouseButtonDown(WidgetGeometry widgetGeometry, PointerEvent &event) override;

    virtual Reply OnMouseMove(WidgetGeometry widgetGeometry, PointerEvent& event) override;

    virtual Reply OnMouseButtonUp(WidgetGeometry widgetGeometry, PointerEvent &event) override;

    int32 GetHandleIndexUnderMouse(vec2 mousePosition, const std::vector<ArrangedWidget> &arrangedChildren);

    void HandleResizing(WidgetGeometry widgetGeometry, vec2 mousePosition);

    void AddChild(const Slot &widget)
    {
        children.push_back(widget);
    }

    void InsertChild(const Slot &widget, uint32 index)
    {
        children.insert(children.begin() + index, widget);
    }

    Slot &GetChild(uint32 index)
    {
        return children[index];
    } 

    void RemoveChild(Ref<Widget> widget)
    {
        children.erase(std::find_if(children.begin(), children.end(), [&](const Slot &slot) {
            return slot.widget == widget;
        }));
    }

    void RemoveChildAt(uint32 index)
    {
        children.erase(children.begin() + index);
    }

    LayoutRule GetLayoutRule() const { return layoutRule; }

    void SetLayoutRule(LayoutRule layoutRule) 
    {
        this->layoutRule = layoutRule;
    }

  protected:
    float GetAlongAxis(vec2 value) { return (layoutRule == LayoutRule::Horizontal) ? value.x : value.y; }

    static constexpr float splitterHandleSize = 5.0f;
    static constexpr float hitDetectionHandleSize = 10.0f;
    static constexpr int32 INDEX_NONE = -1;

    bool isResizing = false;
    int32 hoveredHandleIndex = INDEX_NONE;

    LayoutRule layoutRule;

    std::vector<Slot> children;
};

}