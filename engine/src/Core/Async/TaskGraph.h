#pragma once
#include "Core/Async/Thread.h"

namespace Zafkiel
{

class TaskGraph : public Singleton<TaskGraph, true>
{
  public:
    TaskGraph();
    ~TaskGraph();

    void StartRenderThread();
    void StopRenderThread();

    void EnqueueTask(NamedThreadType type, Task &&task);

  private:
    void StartNamedThread(NamedThreadType type);
    void StopNamedThread(NamedThreadType type);

    std::vector<Scope<NamedThread>> namedThreads;
};

}