#include "engine.h"

#include "function/window/window.h"
#include "function/render/backends/opengl/opengl_context.h"
#include "function/scene/scene.h"
#include "function/script/script_engine.h"

namespace Zafkiel
{
Ref<GraphicsContext> Engine::graphicsContext;
Ref<Scene> Engine::activeScene;
Ref<ScriptEngine> Engine::scriptEngine;

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
void Engine::SetActiveScene(Ref<Scene> scene)
{
    activeScene = scene;
}
Ref<Scene> Engine::GetActiveScene()
{
    return activeScene;
}
Ref<ScriptEngine> Engine::GetScriptEngine()
{
    return scriptEngine;
}
void Engine::SetScriptEngine(const Ref<ScriptEngine> &engine)
{
    scriptEngine = engine;
}

void Engine::SubmitToMainThread(std::function<void(void)> func)
{
    std::scoped_lock<std::mutex> lock(mainThreadMutex);
    mainThreadQueue.emplace_back(func);
}

void Engine::ExecuteMainThreadQueue()
{
    for (auto &fn : mainThreadQueue)
        fn();
    mainThreadQueue.clear();
}
}