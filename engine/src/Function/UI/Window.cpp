#include "Function/UI/Window.h"
#include "Core/Async/TaskGraph.h"
#include "Function/UI/UISystem.h"
#include "Function/RHI/RHI.h"
#include "Function/UI/DrawElementList.h"
#include "Function/UI/WidgetGeometryList.h"
#include "Function/Render/Renderer.h"

namespace Zafkiel
{

void Window::DrawWindow(std::vector<DrawElementList> &lists)
{
    DrawElementList selfDrawElementList;
    widgetGeometryList.Clear();

    Draw(WidgetGeometry {vec2(width, height), position}, selfDrawElementList, widgetGeometryList, nullptr, 0);
    lists.push_back(MoveTemp(selfDrawElementList));
    widgetGeometryList.Sort();

    for (auto childWindow : children)
    {
        childWindow->DrawWindow(lists);
    }
}

uint32 Window::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId)
{
    std::vector<ArrangedWidget> output;
    ArrangeChildren(allocatedGeometry, output);

    return content.widget->Draw(output[0].widgetGeometry, drawElementList, widgetGeometryList, this, layerId);
}

void Window::OnWindowResize(uint32 width, uint32 height) 
{
    this->width = width;
    this->height = height;
    TaskGraph::Instance().EnqueueTask(NamedThreadType::RenderThread, [this, width, height]() {
        viewport->Resize(width, height);
    });
}

}