#pragma once
#include "Function/UI/Widget.h"
#include "Function/UI/Slot.h"
#include "Function/UI/LayoutUtils.h"

namespace Zafkiel 
{

class Panel : public Widget
{
  public:
    void Construct() {}  

    class Slot : public WidgetSlotBase<Slot>
    {
      public:
        SizeRule sizeRule;

        HorizontalAlignmentRule horizontalAlignmentRule = HorizontalAlignmentRule::Fill;

        VerticalAlignmentRule verticalAlignmentRule = VerticalAlignmentRule::Fill;

        float growFactor = 1.0f;

        Slot &SetSizeRule(SizeRule sizeRule)
        {
            this->sizeRule = sizeRule;
            return Self();
        }

        Slot &SetHorizontalAlignmentRule(HorizontalAlignmentRule rule)
        {
            this->horizontalAlignmentRule = rule;
            return Self();
        }

        Slot &SetVerticalAlignmentRule(VerticalAlignmentRule rule)
        {
            this->verticalAlignmentRule = rule;
            return Self();
        }

        Slot &SetGrowFactor(float growFactor)
        {
            this->growFactor = growFactor;
            return Self();
        }
    };

    struct Padding
    {
        float left = 0.0f;
        float right = 0.0f;
        float top = 0.0f;
        float bottom = 0.0f;
    };

    struct Space
    {
        float vertical = 0.0f;
        float horizontal = 0.0f;
    };
    
    virtual vec2 GetDesiredSize() const override;

    virtual void ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const override;

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

    Panel *AddChild(const Slot &widget)
    {
        children.push_back(widget);
        return this;
    }

    Slot &GetChild(uint32 index)
    {
        return children[index];
    }

    Panel *SetPadding(Padding padding)
    {
        this->padding = padding;
        return this;
    }

    Panel *SetSize(vec2 size)
    {
        this->size = size;
        return this;
    }

    Panel *SetSpace(Space space)
    {
        this->space = space;
        return this;
    }

    Panel *SetLayoutRule(LayoutRule rule)
    {
        this->layoutRule = rule;
        return this;
    }

    Padding padding;

    Space space;

    vec2 size;

    LayoutRule layoutRule;

    std::vector<Slot> children;
};

}