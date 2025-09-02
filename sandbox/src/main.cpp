#include "main.h"
#include "refl_generate.h"
#include "core/meta/reflection/refl.h"
#include <function/scene/world.h>
#include <function/scene/components.h>
#include <function/script/script_engine.h>

using namespace Zafkiel;

struct Person
{
    int age;
};

int main(int argc, char **argv)
{
    ReflectionGenerate::RegisterReflectionInfo();
    Log::CoreInfo("Hello,world!");
    ScriptEngine::Init();

    World world;
    auto entity = world.SpawnEntity(ScriptComponent{{"Sandbox.Player"}},
        TransformComponent{vec3(1.0f, 2.0f, 3.0f), vec3(1.0f, 1.0f, 1.0f)});

    ScriptEngine::OnRuntimeStart(world);

    ScriptEngine::OnRuntimeStop();

    ScriptEngine::Shutdown();
    return 0;
}