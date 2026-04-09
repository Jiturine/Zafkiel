#pragma once

namespace Zafkiel
{

struct TaskBase 
{
    virtual ~TaskBase() = default;
    virtual void Run() = 0;
};

template <typename Fn>
struct Task : TaskBase
{
    Fn fn;
    Task(Fn &&f) : fn(MoveTemp(f)) {}
    virtual void Run() override { fn(); }
};

}