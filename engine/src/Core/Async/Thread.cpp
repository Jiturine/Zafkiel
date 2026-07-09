#include "Core/Async/Thread.h"

namespace Zafkiel
{

void NamedThread::StartNewThread()
{
    if (started)
    {
        Log::Error("Thread already started!");
        return;
    }
    handle = std::thread([this]() {
        this->threadId = std::this_thread::get_id();
        
        {
            std::unique_lock lock(initMutex);
            this->attached = true;
        }
        this->initCV.notify_one();

        this->ProcessThreadUntilQuit();
    });

    started = true;
    {
        std::unique_lock lock(initMutex);
        initCV.wait(lock, [&] { return attached.load(); });
    }
}

void NamedThread::AttachCurrentThread()
{
    if (started)
    {
        Log::Error("Thread already started!");
        return;
    }
    threadId = std::this_thread::get_id();
    started = true;
    attached = true;
}

void NamedThread::RequestQuit()
{
    queue.RequestQuit();
}

void NamedThread::ProcessThreadUntilQuit()
{
    queue.ResetQuitFlag();
    queue.EnterRecursionGuard();

    while (!queue.IsQuitRequested())
    {
        Task task = queue.Dequeue();
        if (task) task();
    }

    queue.LeaveRecursionGuard();
}

void NamedThread::EnqueueTask(Task &&task)
{
    queue.Enqueue(MoveTemp(task));
}

}