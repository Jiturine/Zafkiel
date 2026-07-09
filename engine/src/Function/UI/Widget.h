#pragma once
#include "Function/UI/InputEvent.h"
#include "Function/UI/Reply.h"
#include "Function/UI/Visibility.h"

namespace Zafkiel 
{

class DrawElementList;

class WidgetGeometryList;

class Widget;

struct WidgetGeometry
{
    vec2 size;

    vec2 position;

    bool Contain(vec2 point) 
    { 
        return point.x >= position.x && point.x <= position.x + size.x
            && point.y >= position.y && point.y <= position.y + size.y;
    }

    vec2 Center()
    {
        return position + size * 0.5f;
    }
};

struct ArrangedWidget
{
    Ref<Widget> widget;

    WidgetGeometry widgetGeometry;
};

struct WidgetPersistentState
{
    WidgetGeometry allocatedGeometry;
    WeakRef<Widget> paintParent;
};

class Widget : public RefCounted
{
  public:
    uint32 Draw(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, Ref<Widget> parent, uint32 layerId);

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) { return layerId; }

    virtual void ArrangeChildren(WidgetGeometry allocatedGeometry, std::vector<ArrangedWidget> &arrangedChildren) const {}

    virtual Reply OnMouseButtonDown(WidgetGeometry widgetGeometry, PointerEvent &event) { return Reply::Unhandled(); }

    virtual Reply OnMouseMove(WidgetGeometry widgetGeometry, PointerEvent& event) { return Reply::Unhandled(); }

    virtual Reply OnMouseEnter(WidgetGeometry widgetGeometry, PointerEvent& event) { return Reply::Unhandled(); }

    virtual Reply OnMouseLeave(WidgetGeometry widgetGeometry, PointerEvent& event) { return Reply::Unhandled(); }

    virtual Reply OnMouseButtonUp(WidgetGeometry widgetGeometry, PointerEvent &event) { return Reply::Unhandled(); }

    virtual Reply OnDragDetected(WidgetGeometry widgetGeometry, PointerEvent &event, vec2 startPosition) { return Reply::Unhandled(); }

    virtual Reply OnDragEnter(WidgetGeometry widgetGeometry, DragDropEvent &event) { return Reply::Unhandled(); }

	virtual Reply OnDragLeave(WidgetGeometry widgetGeometry, DragDropEvent &event) { return Reply::Unhandled(); }

	virtual Reply OnDragOver(WidgetGeometry widgetGeometry, DragDropEvent &event) { return Reply::Unhandled();}

    virtual Reply OnDrop(WidgetGeometry widgetGeometry, DragDropEvent &event) { return Reply::Unhandled(); }

    virtual CursorReply OnCursorQuery(WidgetGeometry widgetGeometry, PointerEvent &event) { return CursorReply::Unhandled(); }
  
    virtual vec2 GetDesiredSize() const = 0;

    bool HasMouseCapture();

    WidgetGeometry GetPaintGeometry() const { return persistentState.allocatedGeometry; }

    Ref<Widget> GetPaintParent() const { return persistentState.paintParent.Lock(); }

    const Visibility &GetVisibility() const { return visibility; }

    void SetVisibility(Visibility inVisibility) { visibility = inVisibility; }

  protected:
    mutable WidgetPersistentState persistentState;

    Visibility visibility = Visibility::Visible;
};

struct WidgetPath
{
    std::vector<ArrangedWidget> widgets;

    void Add(const ArrangedWidget &widget)
    {
        widgets.push_back(widget);
    }

    void Insert(const ArrangedWidget &widget, uint32 index)
    {
        widgets.insert(widgets.begin() + index, widget);
    }

    void Reverse()
    {
        std::reverse(widgets.begin(), widgets.end());
    }

    ArrangedWidget FindArrangedWidget(Ref<Widget> widget) const
    {
        for (auto &arrangedWidget : widgets)
        {
            if (arrangedWidget.widget == widget)
                return arrangedWidget;
        }
        return {};
    }

    bool ContainsWidget(const Ref<Widget> &widget) const
    {
        for (auto &arrangedWidget : widgets)
        {
            if (arrangedWidget.widget == widget)
                return true;
        }
        return false;
    }
};

struct WeakWidgetPath
{
    WeakWidgetPath() = default;

    WeakWidgetPath(const WidgetPath &widgetPath)
    {
        for (auto &arrangedWidget : widgetPath.widgets)
        {
            widgets.push_back(arrangedWidget.widget);
        }
    }

    std::vector<WeakRef<Widget>> widgets;

    WidgetPath ToWidgetPath();

    bool ContainsWidget(const Ref<Widget> &widget) const
    {
        for (auto &weakWidget : widgets)
        {
            if (weakWidget.Lock() == widget)
                return true;
        }
        return false;
    }
};

template <typename T, typename... Args>
Ref<T> CreateWidget(Args&&... args)
{
    auto widget = CreateRef<T>();
    widget->Construct(std::forward<Args>(args)...);
    return widget;
}

}