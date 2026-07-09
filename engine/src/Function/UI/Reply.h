#pragma once
#include "Function/UI/InputEvent.h"
#include "Platform/Cursor.h"
#include "Platform/Input/Keycode.h"

namespace Zafkiel
{

class Widget;

class Reply
{
  public:
    Reply(bool isHandled) : isHandled(isHandled) {}

    static Reply Unhandled() { return Reply(false); }

    static Reply Handled() { return Reply(true); }

    Reply &CaptureMouse(const Ref<Widget> &widget) 
    {
        mouseCaptor = widget;
        return *this;
    }

    Reply &DetectDrag(const Ref<Widget> &widget, MouseButton button)
    {
        dragDetector = widget;
        dragDetectMouseButton = button;
        return *this;
    }

    Reply &BeginDragDrop(const Ref<DragDropOperation> &operation)
    {
        dragDropOperation = operation;
        return *this;
    }

    Reply &ReleaseMouseCapture()
    {
        mouseCaptor = nullptr;
        releaseMouseCapture = true;
        return *this;
    }

    Ref<Widget> GetMouseCaptor() const { return mouseCaptor; }

    Ref<Widget> GetDragDetector() const { return dragDetector; }

    MouseButton GetDragDetectMouseButton() const { return dragDetectMouseButton; }

    Ref<DragDropOperation> GetDragDropOperation() const { return dragDropOperation; }

    bool IsHandled() const { return isHandled; }

    bool NeedReleaseMouseCapture() const { return releaseMouseCapture; }

  private:
    bool isHandled;

    bool releaseMouseCapture = false;

    Ref<Widget> mouseCaptor;

    Ref<Widget> dragDetector;

    Ref<DragDropOperation> dragDropOperation;

    MouseButton dragDetectMouseButton;
};

class CursorReply
{
  public:
    CursorReply(bool isHandled, CursorType cursorType)
        : isHandled(isHandled), cursorType(cursorType) {}

    static CursorReply Cursor(CursorType cursorType) 
    {
        return CursorReply(true, cursorType);
    }

    static CursorReply Unhandled()
    {
        return CursorReply(false, CursorType::Default);
    }

    bool IsHandled() const { return isHandled; }

    CursorType GetCursorType() const { return cursorType; }

  private:
    bool isHandled;

    CursorType cursorType;
};

}