#pragma once
#include <SDL3/SDL.h>

class Scene;

namespace Zafkiel
{

class GraphicsContext;
class Window;
class Scene;
class ScriptEngine;
class AssetManager;

class Engine
{
  public:
    static Ref<GraphicsContext> CreateGraphicsContext(SDL_Window *window);
    static Ref<GraphicsContext> GetGraphicsContext();
    static Ref<AssetManager> GetAssetManager();

    static Ref<Scene> CreateScene();
    static void SetActiveScene(Ref<Scene> scene);
    static Ref<Scene> GetActiveScene();

    static Ref<ScriptEngine> GetScriptEngine();
    static void SetScriptEngine(const Ref<ScriptEngine> &engine);
    static void SetAssetManager(const Ref<AssetManager> &assetManager);
    static void SubmitToMainThread(std::function<void(void)> func);
    static void ExecuteMainThreadQueue();

  private:
    static Ref<GraphicsContext> graphicsContext;
    static Ref<Scene> activeScene;
    static Ref<ScriptEngine> scriptEngine;
    static Ref<AssetManager> assetManager;
    inline static std::mutex mainThreadMutex;
    inline static std::vector<std::function<void(void)>> mainThreadQueue;
};
}