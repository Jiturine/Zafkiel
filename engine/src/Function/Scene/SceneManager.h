#pragma once
#include "Function/Scene/Scene.h"

namespace Zafkiel 
{

class SceneManager 
{
  public:
    virtual ~SceneManager() = default;

    virtual Scene &GetActiveScene() const = 0;
  
    static SceneManager &Instance() { return *instancePtr; }

  protected:
    inline static SceneManager *instancePtr = nullptr;
};

}