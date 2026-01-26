#pragma once
#include "function/scene/scene_manager.h"

namespace Zafkiel 
{

class EditorSceneManager final : public SceneManager 
{
  public:
    static void Init()
    {
        instance = new EditorSceneManager;
        SceneManager::Instance().instancePtr = instance;
    }
    static void Destroy()
    {
        delete instance;
        instance = nullptr;
        SceneManager::Instance().instancePtr = nullptr;
    }

    static EditorSceneManager &Instance() { return *instance; }

    virtual Borrow<Scene> GetActiveScene() const override { return Borrow(scenes[activeSceneIndex]); }
    virtual MutBorrow<Scene> GetActiveSceneMut() override { return MutBorrow(scenes[activeSceneIndex]); }
    
    Borrow<Scene> OpenScene(const Path &filePath);

  private:
    inline static EditorSceneManager *instance = nullptr;

    int activeSceneIndex = -1;
    std::vector<Scope<Scene>> scenes;
};

}