#include "script_glue.h"
#include "ECS/components.h"
#include "ECS/entity.h"
#include "entt/entity/fwd.hpp"
#include "debug/log.h"
#include "reflection/register.h"
#include "script/script_engine.h"
#include <mono/metadata/loader.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include "utils/utils.h"
#include "reflection/refl.h"

#define ADD_INTERNAL_CALL(name) mono_add_internal_call("Zafkiel.InternalCalls::" #name, (const void *)name)

namespace Zafkiel
{

static std::unordered_map<MonoType *, std::function<bool(Entity)>> entityHasComponentFuncs;

static std::string MonoStringToCppString(MonoString *string)
{
    char *cStr = mono_string_to_utf8(string);
    std::string res(cStr);
    mono_free(cStr);
    return res;
}

// internal_calls

static void Log_CoreTrace(MonoString *string)
{
    Log::CoreTrace("{}", MonoStringToCppString(string));
}
static void Log_CoreDebug(MonoString *string)
{
    Log::CoreDebug("{}", MonoStringToCppString(string));
}
static void Log_CoreInfo(MonoString *string)
{
    Log::CoreInfo("{}", MonoStringToCppString(string));
}
static void Log_CoreWarn(MonoString *string)
{
    Log::CoreWarn("{}", MonoStringToCppString(string));
}
static void Log_CoreError(MonoString *string)
{
    Log::CoreError("{}", MonoStringToCppString(string));
}
static void Log_CoreCritical(MonoString *string)
{
    Log::CoreCritical("{}", MonoStringToCppString(string));
}

static void Log_Trace(MonoString *string)
{
    Log::Trace("{}", MonoStringToCppString(string));
}
static void Log_Debug(MonoString *string)
{
    Log::Debug("{}", MonoStringToCppString(string));
}
static void Log_Info(MonoString *string)
{
    Log::Info("{}", MonoStringToCppString(string));
}
static void Log_Warn(MonoString *string)
{
    Log::Warn("{}", MonoStringToCppString(string));
}
static void Log_Error(MonoString *string)
{
    Log::Error("{}", MonoStringToCppString(string));
}
static void Log_Critical(MonoString *string)
{
    Log::Critical("{}", MonoStringToCppString(string));
}

static bool Entity_HasComponent(EntityID ID, MonoReflectionType *componentType)
{
    MonoType *type = mono_reflection_type_get_type(componentType);
    assert(entityHasComponentFuncs.contains(type));
    Entity entity = ScriptEngine::GetWorldContext()->GetEntityByID(ID);
    return entityHasComponentFuncs[type](entity);
}

// end internal_calls

template <typename T>
static void RegisterComponent()
{
    std::string typeName = Reflection::GetType<T>()->GetName();
    std::string managedName = std::format("Zafkiel.{}", typeName);
    Utils::Replace(managedName, "Component", "");
    MonoType *type = mono_reflection_type_from_name(managedName.data(), ScriptEngine::GetCoreAssemblyImage());
    entityHasComponentFuncs[type] = [](Entity entity) { return entity.HasComponent<T>(); };
}

void ScriptGlue::RegisterComponents()
{
    RegisterComponent<TransformComponent>();
}

void ScriptGlue::AddInternalCalls()
{
    ADD_INTERNAL_CALL(Entity_HasComponent);
    ADD_INTERNAL_CALL(Log_CoreTrace);
    ADD_INTERNAL_CALL(Log_CoreDebug);
    ADD_INTERNAL_CALL(Log_CoreInfo);
    ADD_INTERNAL_CALL(Log_CoreWarn);
    ADD_INTERNAL_CALL(Log_CoreError);
    ADD_INTERNAL_CALL(Log_CoreCritical);
    ADD_INTERNAL_CALL(Log_Trace);
    ADD_INTERNAL_CALL(Log_Debug);
    ADD_INTERNAL_CALL(Log_Info);
    ADD_INTERNAL_CALL(Log_Warn);
    ADD_INTERNAL_CALL(Log_Error);
    ADD_INTERNAL_CALL(Log_Critical);
}

}