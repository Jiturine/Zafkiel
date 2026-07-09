#pragma once

namespace Zafkiel 
{

class Widget;

enum class SizeRule
{
    Auto,
    Grow,
};

enum class HorizontalAlignmentRule
{
    Fill,
    Left,
    Center,
    Right,
};

enum class VerticalAlignmentRule
{
    Fill,
    Top,
    Center,
    Bottom,
};

struct Padding
{
    float top = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
    float right = 0.0f;
};

template <typename DerivedSlot>
class WidgetSlotBase
{
  public:
    Ref<Widget> widget;

    bool IsValid() const { return widget != nullptr; }

    DerivedSlot &operator[](Ref<Widget> &&inWidget)
    {
        widget = MoveTemp(inWidget);
        return Self();
    }

    DerivedSlot &operator[](const Ref<Widget> &inWidget)
    {
        widget = inWidget;
        return Self();
    }

  protected:
    DerivedSlot &Self()
    {
        return *static_cast<DerivedSlot *>(this);
    }
};

class SingleWidgetSlot : public WidgetSlotBase<SingleWidgetSlot>
{
  public:
    SizeRule sizeRule;

    HorizontalAlignmentRule horizontalAlignmentRule = HorizontalAlignmentRule::Fill;

    VerticalAlignmentRule verticalAlignmentRule = VerticalAlignmentRule::Fill;

    Padding padding;

    SingleWidgetSlot &SetSizeRule(SizeRule sizeRule)
    {
        this->sizeRule = sizeRule;
        return Self();
    }

    SingleWidgetSlot &SetHorizontalAlignmentRule(HorizontalAlignmentRule rule)
    {
        this->horizontalAlignmentRule = rule;
        return Self();
    }

    SingleWidgetSlot &SetVerticalAlignmentRule(VerticalAlignmentRule rule)
    {
        this->verticalAlignmentRule = rule;
        return Self();
    }

    SingleWidgetSlot &SetPadding(Padding padding)
    {
        this->padding = padding;
        return Self();
    }
};

}