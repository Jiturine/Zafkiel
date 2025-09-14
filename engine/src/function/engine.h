#pragma once
#include <SDL3/SDL.h>

class Scene;

namespace Zafkiel
{

class GraphicsContext;
class Window;
class Scene;

class Engine
{
  public:
    static Ref<GraphicsContext> CreateGraphicsContext(SDL_Window *window);
    static Ref<GraphicsContext> GetGraphicsContext();

    static Ref<Scene> CreateScene();
    static void SetCurrentScene(Ref<Scene> scene);
    static Ref<Scene> GetCurrentScene();

  private:
    static Ref<GraphicsContext> graphicsContext;
    static Ref<Scene> currentScene;
};
}