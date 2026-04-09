#pragma once
#include "Function/Scene/SceneManager.h"

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

    virtual Scene &GetActiveScene() const override { return *scenes[activeSceneIndex].get(); }
    
    Scene &OpenScene(const Path &filePath);

  private:
    inline static EditorSceneManager *instance = nullptr;

    int activeSceneIndex = -1;
    std::vector<Scope<Scene>> scenes;
};

}