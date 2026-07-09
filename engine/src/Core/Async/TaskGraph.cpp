#include "Core/Async/TaskGraph.h"

namespace Zafkiel
{
TaskGraph::TaskGraph()
{
    namedThreads.resize(3);
    namedThreads[0] = CreateScope<NamedThread>(NamedThreadType::GameThread);
    namedThreads[1] = CreateScope<NamedThread>(NamedThreadType::RenderThread);

    namedThreads[0]->AttachCurrentThread();
}

TaskGraph::~TaskGraph()
{

}

void TaskGraph::StartRenderThread()
{
    StartNamedThread(NamedThreadType::RenderThread);
}
void TaskGraph::StopRenderThread()
{
    StopNamedThread(NamedThreadType::RenderThread);
}

void TaskGraph::EnqueueTask(NamedThreadType type, Task &&task)
{
    int threadIndex = static_cast<int>(type);
    namedThreads[threadIndex]->EnqueueTask(MoveTemp(task));
}

void TaskGraph::StartNamedThread(NamedThreadType type)
{
    int threadIndex = static_cast<int>(type);
    namedThreads[threadIndex]->StartNewThread();
}
void TaskGraph::StopNamedThread(NamedThreadType type)
{
    int threadIndex = static_cast<int>(type);
    namedThreads[threadIndex]->RequestQuit();
}

  
}