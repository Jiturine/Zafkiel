#pragma once
#include "function/scene/scene.h"

namespace Zafkiel 
{

class SceneManager 
{
  public:
    virtual ~SceneManager() = default;

    virtual Borrow<Scene> GetActiveScene() const = 0;
    virtual MutBorrow<Scene> GetActiveSceneMut() = 0;
  
    static SceneManager &Instance() { return *instancePtr; }

  protected:
    inline static SceneManager *instancePtr = nullptr;
};

}