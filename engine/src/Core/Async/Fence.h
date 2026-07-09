#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <memory>

namespace Zafkiel
{

class Fence
{
  public:
    Fence() : completed(false), frameNumber(0) {}

    void Signal()
    {
        std::unique_lock lock(mutex);
        completed = true;
        condition.notify_one();
    }

    void Wait()
    {
        std::unique_lock lock(mutex);
        while (!completed)
        {
            condition.wait(lock);
        }
    }

    bool IsCompleted() const
    {
        return completed.load();
    }

    void Reset()
    {
        std::unique_lock lock(mutex);
        completed = false;
        frameNumber++;
    }

    uint64 GetFrameNumber() const { return frameNumber; }

  private:
    std::atomic<bool> completed;
    std::mutex mutex;
    std::condition_variable condition;
    uint64 frameNumber;
};

}