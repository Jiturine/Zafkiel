#include "engine.h"

#include "function/window/window.h"
#include "function/render/backends/opengl/opengl_context.h"
#include "function/scene/scene.h"

namespace Zafkiel
{
Ref<GraphicsContext> Engine::graphicsContext;
Ref<Scene> Engine::currentScene;

Ref<GraphicsContext> Engine::CreateGraphicsContext(SDL_Window *window)
{
    graphicsContext = MakeRef<OpenGLContext>(window);
    return graphicsContext;
}
Ref<GraphicsContext> Engine::GetGraphicsContext() { return graphicsContext; }

Ref<Scene> Engine::CreateScene()
{
    return MakeRef<Scene>();
}
void Engine::SetCurrentScene(Ref<Scene> scene)
{
    currentScene = scene;
}
Ref<Scene> Engine::GetCurrentScene()
{
    return currentScene;
}

}