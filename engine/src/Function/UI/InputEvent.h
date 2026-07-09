#pragma once
#include "Platform/Input/Keycode.h"

namespace Zafkiel
{
class Widget;

class InputEvent
{
  public:


};

class WidgetPath;

class PointerEvent : public InputEvent
{
  public:
    PointerEvent(vec2 position, MouseButton effectingButton, const std::set<MouseButton> &pressedButtons)
        : position(position), effectingButton(effectingButton), pressedButtons(pressedButtons) {}

    void SetWidgetPath(const WidgetPath *path) { widgetPath = path; }

    void SetDeltaPosition(vec2 delta) { deltaPosition = delta; }

    vec2 GetPosition() const { return position; }

    MouseButton GetEffectingButton() const { return effectingButton; }

    const std::set<MouseButton> &GetPressedButtons() const { return pressedButtons; }

    bool IsMouseButtonPressed(MouseButton button) const { return pressedButtons.contains(button); }

    vec2 GetDeltaPosition() const { return deltaPosition; }

    const WidgetPath *GetWidgetPath() const { return widgetPath; }

  private:
    vec2 position;

    MouseButton effectingButton;

    std::set<MouseButton> pressedButtons;

    vec2 deltaPosition;

    const WidgetPath *widgetPath;
};

class DragDropEvent;

class DragDropOperation : public RefCounted
{
  public:
    virtual Ref<Widget> GetDecorator() const { return nullptr; }

    virtual void OnDragged(const DragDropEvent &dragDropEvent) {}

    virtual void OnDrop(bool handled, const DragDropEvent &dragDropEvent) {}
};

class DragDropEvent : public PointerEvent
{
  public:
    DragDropEvent(PointerEvent &event, const Ref<DragDropOperation> &dragDropOperation)
        : PointerEvent(event), operation(dragDropOperation) {}

    Ref<DragDropOperation> GetOperation() const { return operation; }

  private:
    Ref<DragDropOperation> operation;
};

}