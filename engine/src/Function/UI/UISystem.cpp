#include "Function/UI/UISystem.h"
#include "Function/UI/Window.h"

namespace Zafkiel
{

UISystem::UISystem()
{
    defaultFont = CreateRef<Font>("assets/fonts/HarmonyOS_Sans_SC/HarmonyOS_Sans_SC_Regular.ttf");
}

WidgetPath UISystem::LocateWidgetsInWindow(Ref<Window> window, vec2 screenPosition)
{
    auto &widgetGeometryList = window->GetWidgetGeometryList();

    WidgetPath widgetPath;

    Ref<Widget> curWidget = nullptr;

    for (auto &element : std::views::reverse(widgetGeometryList.GetElements()))
    {
        if (element.widget->GetVisibility().IsHitTestVisible())
        {
            auto &widgetGeometry = element.widgetGeometry;

            if (widgetGeometry.Contain(screenPosition))
            {
                curWidget = element.widget;
                break;
            }
        }
    }

    while (curWidget)
    {
        widgetPath.Add(ArrangedWidget{curWidget, curWidget->GetPaintGeometry()});
        curWidget = curWidget->GetPaintParent();
    }

    widgetPath.Reverse(); // window -> ... -> 被点击的widget

    // TODO: 考虑移除被禁用的widget

    return widgetPath;
}

void UISystem::OnMouseButtonDown(Ref<PlatformWindow> platformWindow, vec2 position, MouseButton button)
{
    pressedMouseButtons.insert(button);

    PointerEvent event(position, button, pressedMouseButtons);

    if (!mouseCaptor)
    {
        auto window = FindWindowByPlatformWindow(platformWindow);
        WidgetPath widgetPath = LocateWidgetsInWindow(window, position);
        RouteEvent(event, widgetPath, [&event](ArrangedWidget &arrangedWidget) {
            return arrangedWidget.widget->OnMouseButtonDown(arrangedWidget.widgetGeometry, event);
        });
    }
    else
    {
        RouteEvent(event, mouseCaptorPath, [&event](ArrangedWidget &arrangedWidget) {
            return arrangedWidget.widget->OnMouseButtonDown(arrangedWidget.widgetGeometry, event);
        });
    }
}

void UISystem::OnMouseMove(Ref<PlatformWindow> platformWindow, vec2 position)
{
    PointerEvent event(position, MouseButton::None, pressedMouseButtons);
    event.SetDeltaPosition(position - lastCursorPosition);

    if (!dragDropOperation) // 已开始拖拽，则不该检测新的拖拽
    {
        if (DetectDrag(event))
        {
            lastWidgetsUnderCursor = dragDetectionState.dragDetectorPath;
            WidgetPath dragRoutePath = dragDetectionState.dragDetectorPath.ToWidgetPath();
            RouteEvent(event, dragRoutePath, [this, &event](ArrangedWidget &arrangedWidget) {
                return arrangedWidget.widget->OnDragDetected(arrangedWidget.widgetGeometry, event, dragDetectionState.startPosition);
            });
            dragDetectionState.dragDetector = nullptr;
        }
    }

    auto window = FindWindowByPlatformWindow(platformWindow);
    WidgetPath widgetPath = LocateWidgetsInWindow(window, position);
    
    DragDropEvent dragDropEvent(event, dragDropOperation);
    for (auto &widget : lastWidgetsUnderCursor.widgets | std::views::reverse)
    {
        if (widget.IsValid())
        {
            auto actualWidget = widget.Lock();
            if (!widgetPath.ContainsWidget(actualWidget))
            {
                ArrangedWidget arrangedWidget{actualWidget, actualWidget->GetPaintGeometry()};
                if (dragDropOperation)
                {
                    arrangedWidget.widget->OnDragLeave(arrangedWidget.widgetGeometry, dragDropEvent);
                }
                else
                {
                    arrangedWidget.widget->OnMouseLeave(arrangedWidget.widgetGeometry, event);
                }
            }
        }
    }

    if (!mouseCaptor)
    {
        RouteEvent(event, widgetPath, [this, &event](ArrangedWidget &arrangedWidget) {
            if (dragDropOperation)
            {
                if (!lastWidgetsUnderCursor.ContainsWidget(arrangedWidget.widget))
                {
                    DragDropEvent dragDropEvent(event, dragDropOperation);
                    return arrangedWidget.widget->OnDragEnter(arrangedWidget.widgetGeometry, dragDropEvent);
                }
                return Reply::Unhandled();
            }
            else
            {
                if (!lastWidgetsUnderCursor.ContainsWidget(arrangedWidget.widget))
                {
                    return arrangedWidget.widget->OnMouseEnter(arrangedWidget.widgetGeometry, event);
                }
                return Reply::Unhandled();
            }
        });

        RouteEvent(event, widgetPath, [this, &event](ArrangedWidget &arrangedWidget) {
            if (dragDropOperation)
            {
                DragDropEvent dragDropEvent(event, dragDropOperation);
                return arrangedWidget.widget->OnDragOver(arrangedWidget.widgetGeometry, dragDropEvent);
            }
            else
            {
                return arrangedWidget.widget->OnMouseMove(arrangedWidget.widgetGeometry, event);
            }
        });
    }
    else 
    {
        RouteEvent(event, mouseCaptorPath, [this, &event](ArrangedWidget &arrangedWidget) {
            if (!lastWidgetsUnderCursor.ContainsWidget(arrangedWidget.widget))
            {
                return arrangedWidget.widget->OnMouseEnter(arrangedWidget.widgetGeometry, event);
            }
            return Reply::Unhandled();
        });

        RouteEvent(event, mouseCaptorPath, [&event](ArrangedWidget &arrangedWidget) {
            return arrangedWidget.widget->OnMouseMove(arrangedWidget.widgetGeometry, event);
        });
    }

    if (dragDropOperation)
    {
        dragDropOperation->OnDragged(dragDropEvent);
    }

    lastCursorPosition = position;
    lastWidgetsUnderCursor = WeakWidgetPath(widgetPath);
}

void UISystem::OnMouseButtonUp(Ref<PlatformWindow> platformWindow, vec2 position, MouseButton button)
{
    pressedMouseButtons.erase(button);
    PointerEvent event(position, button, pressedMouseButtons);

    auto window = FindWindowByPlatformWindow(platformWindow);
    WidgetPath widgetPath = LocateWidgetsInWindow(window, position);

    if (!mouseCaptor)
    {
        Ref<DragDropOperation> tempDragDropOperation = nullptr;
        if (dragDropOperation)
        {
            tempDragDropOperation = dragDropOperation;
            dragDropOperation = nullptr;
        }
        auto reply = RouteEvent(event, widgetPath, [&tempDragDropOperation, &event](ArrangedWidget &arrangedWidget) {
            if (tempDragDropOperation)
            {
                DragDropEvent dragDropEvent(event, tempDragDropOperation);
                return arrangedWidget.widget->OnDrop(arrangedWidget.widgetGeometry, dragDropEvent);
            }
            else
            {
                return arrangedWidget.widget->OnMouseButtonUp(arrangedWidget.widgetGeometry, event);
            }
        });
        if (tempDragDropOperation)
        {
            DragDropEvent dragDropEvent(event, tempDragDropOperation);
            tempDragDropOperation->OnDrop(reply.IsHandled(), dragDropEvent);
        }
    }
    else
    {
        RouteEvent(event, mouseCaptorPath, [&event](ArrangedWidget &arrangedWidget) {
            return arrangedWidget.widget->OnMouseButtonUp(arrangedWidget.widgetGeometry, event);
        });
    }
}

Ref<Window> UISystem::FindWindowByPlatformWindow(Ref<PlatformWindow> platformWindow)
{
    for (auto &window : windows)
    {
        if (window->GetNativeWindow()->GetHandle() == platformWindow->GetHandle())
        {
            return window;
        }
    }
    return nullptr;
}

void UISystem::OnWindowResize(Ref<PlatformWindow> platformWindow, uint32 width, uint32 height)
{
    platformWindow->SetWidth(width);
    platformWindow->SetHeight(height);
    
    auto window = FindWindowByPlatformWindow(platformWindow);
    
    if (window)
    {
        window->OnWindowResize(width, height);
    }
}

Reply UISystem::RouteEvent(PointerEvent &event, WidgetPath &widgetPath, std::function<Reply(ArrangedWidget &)> func)
{
    event.SetWidgetPath(&widgetPath);

    Reply reply = Reply::Unhandled();
    for (auto &widget : std::views::reverse(widgetPath.widgets))
    {
        reply = func(widget);
        ProcessReply(reply, widgetPath, event);

        if (reply.IsHandled()) break;
    }
    return reply;
}

void UISystem::ProcessReply(const Reply &reply, WidgetPath &routingPath, PointerEvent &pointerEvent)
{
    if (auto replyMouseCaptor = reply.GetMouseCaptor())
    {
        mouseCaptor = replyMouseCaptor;
        mouseCaptorPath = routingPath;
    }
    else if (reply.NeedReleaseMouseCapture())
    {
        mouseCaptor = nullptr;
        mouseCaptorPath.widgets.clear();
    }

    if (auto replyDragDetector = reply.GetDragDetector())
    {
        dragDetectionState.dragDetector = replyDragDetector;
        dragDetectionState.dragDetectorPath = routingPath;
        dragDetectionState.button = reply.GetDragDetectMouseButton();
        dragDetectionState.startPosition = pointerEvent.GetPosition();
    }

    if (auto replyDragDropOperation = reply.GetDragDropOperation())
    {
        dragDropOperation = replyDragDropOperation;
        auto lastWidgetsPath = lastWidgetsUnderCursor.ToWidgetPath();

        RouteEvent(pointerEvent, lastWidgetsPath, [&pointerEvent](ArrangedWidget &arrangedWidget) {
            return arrangedWidget.widget->OnMouseLeave(arrangedWidget.widgetGeometry, pointerEvent);
        });

        DragDropEvent dragDropEvent(pointerEvent, dragDropOperation);
        RouteEvent(pointerEvent, routingPath, [&dragDropEvent](ArrangedWidget &arrangedWidget) {
            return arrangedWidget.widget->OnDragEnter(arrangedWidget.widgetGeometry, dragDropEvent);
        });
    }
}

void UISystem::QueryCursor()
{
    WidgetPath queryPath;
    if (mouseCaptor)
    {
        queryPath = mouseCaptorPath;   
    }
    else
    {
        auto platformWindowUnderCursor = PlatformApplication::Instance().GetCurrentUnderCursorWindow();
        if (platformWindowUnderCursor) 
        {
            auto window = FindWindowByPlatformWindow(platformWindowUnderCursor);
            queryPath = LocateWidgetsInWindow(window, PlatformApplication::Instance().GetCurrentCursorPosition());;
        }
        else
        {
            for (auto window : std::views::reverse(windows))
            {
                queryPath = LocateWidgetsInWindow(window, PlatformApplication::Instance().GetCurrentCursorPosition());;
                if (!queryPath.widgets.empty()) break;
            }
        }
    }
    CursorReply cursorReply = CursorReply::Unhandled();
    PointerEvent event(PlatformApplication::Instance().GetCurrentCursorPosition(), MouseButton::None, pressedMouseButtons);
    event.SetWidgetPath(&queryPath);
    for (auto &widget : std::views::reverse(queryPath.widgets))
    {
        cursorReply = widget.widget->OnCursorQuery(widget.widgetGeometry, event);
        if (cursorReply.IsHandled()) break;
    }
    ProcessCursorReply(cursorReply);
}

void UISystem::ProcessCursorReply(const CursorReply &cursorReply)
{
    if (cursorReply.IsHandled())
    {
        PlatformApplication::Instance().SetCursor(cursorReply.GetCursorType());
    }
    else
    {
        PlatformApplication::Instance().SetCursor(CursorType::Default);
    }
}

bool UISystem::DetectDrag(const PointerEvent &pointerEvent)
{
    if (dragDetectionState.dragDetector)
    {
		vec2 dragDelta = dragDetectionState.startPosition - pointerEvent.GetPosition();
		if (dragDelta.x * dragDelta.x + dragDelta.y * dragDelta.y > dragTriggerDistance * dragTriggerDistance)
		{
            return true;
		}
    }
    return false;
}

}