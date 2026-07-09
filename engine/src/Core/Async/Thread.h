#pragma once

namespace Zafkiel
{

enum class NamedThreadType
{
    GameThread = 0,
    RenderThread = 1,
};

using Task = std::function<void()>;

class TaskQueue
{
  public:
    void Enqueue(Task &&task)
    {
        std::unique_lock lock(mutex);
        queue.push(MoveTemp(task));
        stallRestartCV.notify_one();
    }

    Task Dequeue()
    {
        std::unique_lock lock(mutex);
        if (queue.empty())
        {
            stallRestartCV.wait(lock, [&] {
                return !queue.empty() || quitRequested.load();
            });
            if (queue.empty()) return {};
            if (quitRequested.load()) return {};
        }
        Task task = MoveTemp(queue.front());
        queue.pop();
        return task;
    }

    void EnterRecursionGuard()
    {
        int32 oldValue = ++recursionGuard;
        if (oldValue != 1)
        {
            Log::Critical("TaskGraph Error: Recursion guard violation (expected 1, got {})", oldValue);
            return;
        }
    }

    void LeaveRecursionGuard()
    {
        int32 oldValue = --recursionGuard;
        if (oldValue != 0)
        {
            Log::Critical("TaskGraph Error: Recursion guard violation (expected 0, got {})", oldValue);
            return;
        }
    }

    void ResetQuitFlag()
    {
        quitRequested.store(false);
    }

    bool IsQuitRequested()
    {
        return quitRequested.load();
    }

    void RequestQuit()
    {
        quitRequested.store(true);
        stallRestartCV.notify_all();
    }

  private:
    std::mutex mutex;
    std::queue<Task> queue;
    std::condition_variable stallRestartCV;
    std::atomic<int32> recursionGuard;
    std::atomic<bool> quitRequested;
};

class NamedThread
{
  public:
    NamedThread(NamedThreadType type)
        : type(type) 
    {
    }
    ~NamedThread()
    {
        if (type != NamedThreadType::GameThread && handle.joinable())
        {
            handle.join();
        }
    }

    void AttachCurrentThread(); // 主线程使用

    void StartNewThread(); // 其他线程使用

    void RequestQuit();

    void EnqueueTask(Task &&task);

    void ProcessThreadUntilQuit();

  private:
    NamedThreadType type;
    TaskQueue queue;
    std::atomic<bool> started = false;
    std::atomic<bool> attached = false;
    std::thread handle;
    std::thread::id threadId;
    std::mutex initMutex;
    std::condition_variable initCV;
};


}