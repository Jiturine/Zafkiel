#pragma once
#include "Function/Render/Font.h"
#include "Function/UI/Widget.h"
#include "Platform/PlatformApplication.h"
#include "Function/UI/Window.h"

namespace Zafkiel
{

class UISystem : public Singleton<UISystem, true>, public PlatformApplicationEventHandler
{
  public:
    UISystem();

    void AddWindow(Ref<Window> window) { windows.push_back(window); }

    void ProcessReply(const Reply &reply, WidgetPath &routingPath, PointerEvent &pointerEvent);

    void ProcessCursorReply(const CursorReply &cursorReply);

    void QueryCursor();

    bool DetectDrag(const PointerEvent &pointerEvent);

    bool WidgetHasMouseCapture(Ref<Widget> widget) const { return widget == mouseCaptor; }

    WidgetPath LocateWidgetsInWindow(Ref<Window> window, vec2 screenPosition);

    virtual void OnWindowResize(Ref<PlatformWindow> window, uint32 width, uint32 height) override;
    
    virtual void OnMouseMove(Ref<PlatformWindow> window, vec2 position) override;
  
    virtual void OnMouseButtonDown(Ref<PlatformWindow> window, vec2 position, MouseButton button) override;

    virtual void OnMouseButtonUp(Ref<PlatformWindow> window, vec2 position, MouseButton button) override;

    void SetPlatformApplication(PlatformApplication *application)
    {
        platformApplication = application;
    }

    void SetPointerCaptor(Ref<Widget> captor, const WidgetPath &widgetPath);

    Ref<Window> FindWindowByPlatformWindow(Ref<PlatformWindow> platformWindow);

    Ref<Font> GetDefaultFont() const { return defaultFont; }

  private:
    Reply RouteEvent(PointerEvent &event, WidgetPath &widgetPath, std::function<Reply(ArrangedWidget &)> func);

    static constexpr float dragTriggerDistance = 10.0f;

    Ref<Widget> mouseCaptor;

    WidgetPath mouseCaptorPath;

    struct DragDetectionState
    {
        Ref<Widget> dragDetector;
        MouseButton button;
        vec2 startPosition;
        WeakWidgetPath dragDetectorPath;
    } dragDetectionState;

    Ref<DragDropOperation> dragDropOperation;

    vec2 lastCursorPosition;

    WeakWidgetPath lastWidgetsUnderCursor;

    std::set<MouseButton> pressedMouseButtons;

    std::vector<Ref<Window>> windows;

    PlatformApplication *platformApplication;

    Ref<Font> defaultFont;
};


}