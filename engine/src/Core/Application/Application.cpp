#include "Core/Application/Application.h"
#include "Core/Time/Time.h"

namespace Zafkiel
{
void Application::PushLayer(Ref<Layer> layer)
{
    layers.push_back(layer);
    layer->OnAttach();
}

void Application::Run()
{
    running = true;
    float time = Time::Now();

    while (running)
    {
        float timestep = Time::Now() - time;
        time = Time::Now();

        for (auto& layer : layers)
        {
            layer->OnUpdate(timestep);
        }

    }

    for (auto &layer : layers)
    {
        layer->OnDetach();
        layer = nullptr;
    }
}
#if 0
void Application::ExecuteMainThreadQueue()
{
    {
        std::scoped_lock lock(mainThreadMutex);
        std::swap(mainThreadSubmitQueue, mainThreadExecuteQueue);
    }

    for (auto& fn : mainThreadExecuteQueue)
        fn->Run();
    mainThreadExecuteQueue.clear();
}

void Application::ExecuteRenderThreadQueue()
{
    {
        std::scoped_lock lock(renderThreadMutex);
        std::swap(renderThreadSubmitQueue, renderThreadExecuteQueue);
    }

    for (auto& fn : renderThreadExecuteQueue)
        fn->Run();
    renderThreadExecuteQueue.clear();
}
#endif
}
