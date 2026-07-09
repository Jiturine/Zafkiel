#pragma once
#include "Function/UI/Panel.h"
#include "Platform/PlatformWindow/PlatformWindow.h"
#include "Function/RHI/RHIResources.h"
#include "Function/UI/CompoundWidget.h"
#include "Function/UI/WidgetGeometryList.h"

namespace Zafkiel
{

class Window : public CompoundWidget
{
  public:
    void Construct(PlatformWindow *nativeWindow)
    {
        this->nativeWindow = nativeWindow;
    }

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;

    void DrawWindow(std::vector<DrawElementList> &lists);

    virtual vec2 GetDesiredSize() const override { return vec2(width, height); }

    void SetSize(uint32 width, uint32 height) { this->width = width, this->height = height; }

    uint32 GetWidth() { return width; }

    uint32 GetHeight() { return height; }

    void SetViewport(RHIViewport *viewport) { this->viewport = viewport; }

    RHIViewport *GetViewport() { return viewport; }

    WidgetGeometryList &GetWidgetGeometryList() { return widgetGeometryList; }

    void OnWindowResize(uint32 width, uint32 height);

    PlatformWindow *GetNativeWindow() { return nativeWindow; }

  private:
    std::vector<Ref<Window>> children;

    WidgetGeometryList widgetGeometryList;

    PlatformWindow *nativeWindow;

    RHIViewport *viewport;

    uint32 width, height;

    vec2 position;
};

}
