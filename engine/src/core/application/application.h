#pragma once
#include "core/base/task.h"
#include "layer.h"

namespace Zafkiel
{
enum class ThreadState 
{
    Idle, Busy, Kick
};

class Application
{
  public:
    virtual ~Application() = default;
    void PushLayer(Ref<Layer> layer);
    void Run();
    void Exit() { running = false; }

    template <typename Fn>
    static void SubmitToMainThread(Fn &&func)
    {
        instance->SubmitToMainThreadImpl(std::forward<Fn>(func));
    }
    static void ExecuteMainThreadQueue()
    {
        instance->ExecuteMainThreadQueueImpl();
    }
    template <typename Fn>
    static void SubmitToRenderThread(Fn &&func)
    {
        instance->SubmitToRenderThreadImpl(std::forward<Fn>(func));
    }
    static void ExecuteRenderThreadQueue()
    {
        instance->ExecuteRenderThreadQueueImpl();
    }
    
    static void StartRenderThread(std::function<void()> initFunc)
    {
        instance->StartRenderThreadImpl(initFunc);
    }
    
    void StartRenderThreadImpl(std::function<void()> initFunc)
    {
        renderThread = std::thread([this, initFunc] (){ 
            initFunc();
            renderThreadInitFinished = true;
            {
                std::unique_lock lock(renderThreadMutex);
                renderThreadConditionVar.notify_one();
            }
            RenderThreadLoop();
        });
    }

    static void StopRenderThread()
    {
        instance->StopRenderThreadImpl();
    }

    void StopRenderThreadImpl()
    {
        renderThreadStop.store(true);
        KickRenderThread();
        if (renderThread.joinable())
        {
            renderThread.join();
        }
    }

    void RenderThreadLoop()
    {
        while (!renderThreadStop.load())
        {
            // wait for signal
            {
                std::unique_lock lock(renderThreadMutex);
                renderThreadConditionVar.wait(lock);
                renderThreadConditionVar.notify_one();
            }

            // execute
            ExecuteRenderThreadQueueImpl();

            // idle
            {
                std::unique_lock lock(renderThreadMutex);
                renderThreadConditionVar.notify_one(); // 提醒主线程
            }
        }
    }

    static void WaitRenderThreadInitFinish()
    {
        instance->WaitRenderThreadInitFinishImpl();
    }

    void WaitRenderThreadInitFinishImpl()
    {
        std::unique_lock lock(renderThreadMutex);
        renderThreadConditionVar.notify_one();
        while (!renderThreadInitFinished) 
        {
            renderThreadConditionVar.wait(lock);   
        }
    }

    static void WaitRenderThread()
    {
        instance->WaitRenderThreadImpl();
    }

    void WaitRenderThreadImpl()
    {
        std::unique_lock lock(renderThreadMutex);
        renderThreadConditionVar.notify_one();
		while (!renderThreadExecuteQueue.empty())
		{
			renderThreadConditionVar.wait(lock);
        }
    }

    static void KickRenderThread() 
    {
        instance->KickRenderThreadImpl();
    }


    void KickRenderThreadImpl()
    {
		std::unique_lock lock(renderThreadMutex);
		renderThreadConditionVar.notify_one();
    }
    
    static void ClearRenderThreadSubmitQueue() 
    {
        instance->ClearRenderThreadSubmitQueueImpl();
    }
    
    void ClearRenderThreadSubmitQueueImpl()
    {
		std::unique_lock lock(renderThreadMutex);
        renderThreadSubmitQueue.clear();
    }

    static Application& Create()
    {
        instance.reset(new Application);
        return *instance;
    }
    static Application& Instance() { return *instance; }

    static ThreadState GetRenderThreadState() { return instance->renderThreadState; }

  private:
    Application() = default;

    template <typename Fn>
    void SubmitToMainThreadImpl(Fn &&func)
    {
        std::scoped_lock<std::mutex> lock(mainThreadMutex);
        mainThreadSubmitQueue.emplace_back(CreateScope<Task<Fn>>(std::forward<Fn>(func)));
    }
    void ExecuteMainThreadQueueImpl();

    template <typename Fn>
    void SubmitToRenderThreadImpl(Fn &&func)
    {
        std::scoped_lock<std::mutex> lock(renderThreadMutex);
        renderThreadSubmitQueue.emplace_back(CreateScope<Task<Fn>>(std::forward<Fn>(func)));
    }
    void ExecuteRenderThreadQueueImpl();

    inline static Scope<Application> instance;
    std::vector<Ref<Layer>> layers;
    std::mutex mainThreadMutex;
    std::vector<Scope<TaskBase>> mainThreadSubmitQueue;
    std::vector<Scope<TaskBase>> mainThreadExecuteQueue;
    std::mutex renderThreadMutex;
    std::vector<Scope<TaskBase>> renderThreadSubmitQueue;
    std::vector<Scope<TaskBase>> renderThreadExecuteQueue;
    std::thread renderThread;
    std::condition_variable renderThreadConditionVar;
    std::atomic<bool> renderThreadInitFinished = false;

    ThreadState renderThreadState = ThreadState::Idle;
    std::atomic<bool> renderThreadStop = false;
    bool running;
};
}
