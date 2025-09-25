#pragma once
#include <SDL3/SDL.h>

class Scene;

namespace Zafkiel
{

class GraphicsContext;
class Window;
class Scene;
class ScriptEngine;

class Engine
{
  public:
    static Ref<GraphicsContext> CreateGraphicsContext(SDL_Window *window);
    static Ref<GraphicsContext> GetGraphicsContext();

    static Ref<Scene> CreateScene();
    static void SetActiveScene(Ref<Scene> scene);
    static Ref<Scene> GetActiveScene();

    static Ref<ScriptEngine> GetScriptEngine();
    static void SetScriptEngine(const Ref<ScriptEngine> &engine);
    static void SubmitToMainThread(std::function<void(void)> func);
    static void ExecuteMainThreadQueue();

  private:
    static Ref<GraphicsContext> graphicsContext;
    static Ref<Scene> activeScene;
    static Ref<ScriptEngine> scriptEngine;
    inline static std::mutex mainThreadMutex;
    inline static std::vector<std::function<void(void)>> mainThreadQueue;
};
}