#pragma once
#include "function/scene/scene_manager.h"

namespace Zafkiel 
{

class EditorSceneManager final : public SceneManager 
{
  public:
    static void Init()
    {
        instance.reset(new EditorSceneManager);
        SceneManager::instancePtr = instance;
    }
    static void Destroy()
    {
        instance = nullptr;
        SceneManager::instancePtr = nullptr;
    }
    
    static Observer<Scene> OpenScene(const Path &filePath) { return instance->OpenSceneImpl(filePath); }

  private:
    inline static Scope<EditorSceneManager> instance;

    Observer<Scene> GetActiveSceneImpl() override { return activeScene; }
    Observer<Scene> OpenSceneImpl(const Path &filePath);

    Observer<Scene> activeScene;
    
};

}