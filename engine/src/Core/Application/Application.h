#pragma once
#include "Core/Application/Layer.h"
#include "Core/Async/Fence.h"

namespace Zafkiel
{

class Application : public Singleton<Application, false>
{
  public:
    virtual ~Application() = default;
    void PushLayer(Ref<Layer> layer);
    void Run();
    void Exit() { running = false; }
    std::vector<Ref<Layer>> layers;

    bool running;
};
}