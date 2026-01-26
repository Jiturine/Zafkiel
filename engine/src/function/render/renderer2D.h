#pragma once
#include "function/render/graphics_context.h"
#include <glad/glad.h>
#include "function/scene/entity.h"

namespace Zafkiel
{
// 区分 Editor 和 Runtime， 在子类进行单例初始化和销毁
class Renderer2D
{
  public:
    virtual ~Renderer2D() = default;
    static Scope<Renderer2D>& Instance() { return instance; }

  protected:
    Renderer2D() = default;
    static Scope<Renderer2D> instance;
};
}