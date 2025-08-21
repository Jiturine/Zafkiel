#pragma once

namespace Zafkiel
{

class GraphicsContext
{
  public:
    virtual ~GraphicsContext() = default;
    virtual void Clear() = 0;
    virtual void SwapBuffers() = 0;
    virtual void *GetHandle() const = 0;
};

}