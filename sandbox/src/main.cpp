#include "main.h"
#include "refl_generate.h"
#include "reflection/register.h"
#include <iostream>
#include <ECS/world.h>
#include <ECS/components.h>
#include <script/script_engine.h>
#include <debug/log.h>

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