#include "function/script/script_glue.h"
#include "function/scene/components.h"
#include "function/scene/entity.h"
#include <entt/entity/fwd.hpp>
#include "mono/utils/mono-publib.h"
#include "function/script/script_engine.h"
#include <mono/metadata/loader.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include "core/meta/reflection/refl.h"
#include "function/scene/scene_manager.h"

#define ADD_INTERNAL_CALL(name) mono_add_internal_call("Zafkiel.InternalCalls::" #name, (const void *)name)

namespace Zafkiel
{

static std::unordered_map<std::string, std::function<bool(Entity)>> entityHasComponentFuncs;

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
    Log::Trace("{}", MonoStringToCppString(string));
}
static void Log_CoreDebug(MonoString *string)
{
    Log::Debug("{}", MonoStringToCppString(string));
}
static void Log_CoreInfo(MonoString *string)
{
    Log::Info("{}", MonoStringToCppString(string));
}
static void Log_CoreWarn(MonoString *string)
{
    Log::Warn("{}", MonoStringToCppString(string));
}
static void Log_CoreError(MonoString *string)
{
    Log::Error("{}", MonoStringToCppString(string));
}
static void Log_CoreCritical(MonoString *string)
{
    Log::Critical("{}", MonoStringToCppString(string));
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

static bool Entity_HasComponent(uint64_t uuid, MonoReflectionType *componentType)
{
    MonoType *type = mono_reflection_type_get_type(componentType);
    std::string typeName = mono_type_get_name(type);
    assert(entityHasComponentFuncs.contains(typeName));
    Entity entity = SceneManager::Instance().Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    return entityHasComponentFuncs[typeName](entity);
}

static bool Entity_HasScript(uint64_t uuid, MonoReflectionType *scriptType)
{
    MonoType *type = mono_reflection_type_get_type(scriptType);
    std::string typeName = mono_type_get_name(type);
    return ScriptEngine::HasScriptInstance(uuid, typeName);
}

static MonoObject *Entity_GetScript(uint64_t uuid, MonoReflectionType *scriptType)
{
    MonoType *type = mono_reflection_type_get_type(scriptType);
    std::string typeName = mono_type_get_name(type);
    return ScriptEngine::GetScriptInstance(uuid, typeName)->GetHandle();
}

static void Entity_GetPosition(uint64_t uuid, vec3 *outPosition)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    *outPosition = entity.GetComponent<TransformComponent>().position;
}

static void Entity_SetPosition(uint64_t uuid, vec3 *position)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    entity.GetComponent<TransformComponent>().SetPosition(*position);
}

static void Entity_GetRotation(uint64_t uuid, quat *outRotation)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    *outRotation = entity.GetComponent<TransformComponent>().rotation;
}

static void Entity_SetRotation(uint64_t uuid, quat *rotation)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    entity.GetComponent<TransformComponent>().SetRotation(*rotation);
}

static void Entity_GetScale(uint64_t uuid, vec3 *outScale)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    *outScale = entity.GetComponent<TransformComponent>().scale;
}

static void Entity_SetScale(uint64_t uuid, vec3 *scale)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    entity.GetComponent<TransformComponent>().SetScale(*scale);
}

static void Transform_GetPosition(uint64_t uuid, vec3 *outPosition)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    *outPosition = entity.GetComponent<TransformComponent>().position;
}

static void Transform_SetPosition(uint64_t uuid, vec3 *position)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    entity.GetComponent<TransformComponent>().SetPosition(*position);
}

static void Transform_GetRotation(uint64_t uuid, quat *outRotation)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    *outRotation = entity.GetComponent<TransformComponent>().rotation;
}

static void Transform_SetRotation(uint64_t uuid, quat *rotation)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);

    entity.GetComponent<TransformComponent>().SetRotation(*rotation);
}

static void Transform_GetScale(uint64_t uuid, vec3 *outScale)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    *outScale = entity.GetComponent<TransformComponent>().scale;
}

static void Transform_SetScale(uint64_t uuid, vec3 *scale)
{
    Entity entity = SceneManager::Instance().GetActiveScene()->GetWorld().GetEntityByUUID(uuid);
    entity.GetComponent<TransformComponent>().SetScale(*scale);
}

// end internal_calls

template <typename T>
static void RegisterComponent()
{
    std::string typeName = Reflection::GetType<T>()->GetName();
    std::string managedName = std::format("Zafkiel.{}", typeName);
    string_replace(managedName, "Component", "");
    entityHasComponentFuncs[managedName] = [](Entity entity) { return entity.HasComponent<T>(); };
}

void ScriptGlue::RegisterComponents()
{
    RegisterComponent<TransformComponent>();
}

void ScriptGlue::AddInternalCalls()
{
    ADD_INTERNAL_CALL(Entity_HasComponent);
    ADD_INTERNAL_CALL(Entity_GetScript);
    ADD_INTERNAL_CALL(Entity_HasScript);
    ADD_INTERNAL_CALL(Entity_GetPosition);
    ADD_INTERNAL_CALL(Entity_SetPosition);
    ADD_INTERNAL_CALL(Entity_GetRotation);
    ADD_INTERNAL_CALL(Entity_SetRotation);
    ADD_INTERNAL_CALL(Entity_GetScale);
    ADD_INTERNAL_CALL(Entity_SetScale);
    ADD_INTERNAL_CALL(Transform_GetPosition);
    ADD_INTERNAL_CALL(Transform_SetPosition);
    ADD_INTERNAL_CALL(Transform_GetRotation);
    ADD_INTERNAL_CALL(Transform_SetRotation);
    ADD_INTERNAL_CALL(Transform_GetScale);
    ADD_INTERNAL_CALL(Transform_SetScale);
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