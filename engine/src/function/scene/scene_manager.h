#pragma once
#include "scene.h"

namespace Zafkiel 
{

class SceneManager 
{
  public:
    virtual ~SceneManager() = default;
    static Observer<SceneManager> Instance() { return instancePtr; }
    static Observer<Scene> GetActiveScene() { return instancePtr->GetActiveSceneImpl(); }

  protected:
    inline static Observer<SceneManager> instancePtr;

    virtual Observer<Scene> GetActiveSceneImpl() = 0;
};

}