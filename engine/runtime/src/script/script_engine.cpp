#include "script_engine.h"
#include "ECS/components.h"
#include "script_glue.h"
#include <cstring>
#include "debug/log.h"
#include <mono/metadata/assembly.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/object.h>

namespace Zafkiel
{

void ScriptEngine::Init()
{
    rootDomain = mono_jit_init("ZafkielJITRuntime");
    assert(rootDomain);
    LoadCoreAssembly("ScriptCore.dll");
    LoadAppAssembly("AppAssembly.dll");
    LoadAssemblyClasses();
    ScriptGlue::AddInternalCalls();
    ScriptGlue::RegisterComponents();
    entityClass = ScriptClass("Zafkiel", "Entity", true);
}

void ScriptEngine::Shutdown()
{
    mono_domain_set(mono_get_root_domain(), false); // 必须先将domain设回rootDomain，再unload Appdomain，否则会崩溃

    mono_domain_unload(appDomain);
    appDomain = nullptr;
    mono_jit_cleanup(rootDomain);
    rootDomain = nullptr;
}

void ScriptEngine::PrintCoreAssemblyTypes()
{
    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(coreAssemblyImage, MONO_TABLE_TYPEDEF);
    size_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    for (size_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char *namespaceStr = mono_metadata_string_heap(coreAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
        const char *nameStr = mono_metadata_string_heap(coreAssemblyImage, cols[MONO_TYPEDEF_NAME]);

        Log::CoreDebug("[[CORE ASSEMBLY]]: {}.{}", namespaceStr, nameStr);
    }
}

void ScriptEngine::PrintAppAssemblyTypes()
{
    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(appAssemblyImage, MONO_TABLE_TYPEDEF);
    size_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    for (size_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char *namespaceStr = mono_metadata_string_heap(appAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
        const char *nameStr = mono_metadata_string_heap(appAssemblyImage, cols[MONO_TYPEDEF_NAME]);

        Log::CoreDebug("[[APP ASSEMBLY]]: {}.{}", namespaceStr, nameStr);
    }
}

void ScriptEngine::LoadAssemblyClasses()
{
    entityClasses.clear();
    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(appAssemblyImage, MONO_TABLE_TYPEDEF);
    size_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    MonoClass *rawEntityClass = mono_class_from_name(coreAssemblyImage, "Zafkiel", "Entity");
    for (size_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char *namespaceStr = mono_metadata_string_heap(appAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
        const char *nameStr = mono_metadata_string_heap(appAssemblyImage, cols[MONO_TYPEDEF_NAME]);
        std::string fullName = (strlen(namespaceStr) != 0) ? std::format("{}.{}", namespaceStr, nameStr) : nameStr;

        MonoClass *monoClass = mono_class_from_name(appAssemblyImage, namespaceStr, nameStr);

        bool isEntity = mono_class_is_subclass_of(monoClass, rawEntityClass, false);

        if (isEntity && monoClass != rawEntityClass)
        {
            entityClasses[fullName] = std::make_shared<ScriptClass>(namespaceStr, nameStr, false);
        }
    }
}

void ScriptEngine::LoadCoreAssembly(const std::filesystem::path &assemblyPath)
{
    appDomain = mono_domain_create_appdomain((char *)"ZafkielScriptRuntime", nullptr);
    assert(appDomain);
    mono_domain_set(appDomain, true);

    coreAssembly = mono_domain_assembly_open(appDomain, assemblyPath.c_str());
    assert(coreAssembly);
    coreAssemblyImage = mono_assembly_get_image(coreAssembly);
    assert(coreAssemblyImage);

    // DEBUG
    PrintCoreAssemblyTypes();
}

void ScriptEngine::LoadAppAssembly(const std::filesystem::path &assemblyPath)
{
    appAssembly = mono_domain_assembly_open(appDomain, assemblyPath.c_str());
    assert(coreAssembly);
    appAssemblyImage = mono_assembly_get_image(appAssembly);
    assert(coreAssemblyImage);

    // DEBUG
    PrintAppAssemblyTypes();
}

void ScriptEngine::OnRuntimeStart(World &world)
{
    worldContext = &world;
    for (auto entity : world.Query<ScriptComponent>())
    {
        const auto &scriptComponent = entity.GetComponent<ScriptComponent>();
        for (const auto &scriptName : scriptComponent.scripts)
        {
            if (entityClasses.contains(scriptName))
            {
                auto instance = std::make_shared<ScriptInstance>(entityClasses[scriptName], entity.GetHandle());
                entityInstances[entity.GetHandle()][scriptName] = instance;
                instance->InvokeOnCreate();
            }
        }
    }
}

void ScriptEngine::OnRuntimeStop()
{
    entityInstances.clear();
}

MonoObject *ScriptEngine::InstantiateClass(MonoClass *monoClass)
{
    MonoObject *instance = mono_object_new(appDomain, monoClass);
    assert(instance);
    mono_runtime_object_init(instance);
    return instance;
}

ScriptClass::ScriptClass(const std::string &classNamespace, const std::string &className, bool isCore)
    : classNamespace(classNamespace), className(className)
{
    monoClass = mono_class_from_name(isCore ? ScriptEngine::GetCoreAssemblyImage() : ScriptEngine::GetAppAssemblyImage(), classNamespace.c_str(), className.c_str());
}

MonoObject *ScriptClass::Instantiate()
{
    return ScriptEngine::InstantiateClass(monoClass);
}

MonoMethod *ScriptClass::GetMethod(const std::string &name, int paramCount)
{
    return mono_class_get_method_from_name(monoClass, name.c_str(), paramCount);
}

MonoObject *ScriptClass::InvokeMethod(MonoObject *instance, MonoMethod *method, void **params)
{
    return mono_runtime_invoke(method, instance, params, nullptr);
}

ScriptInstance::ScriptInstance(const std::shared_ptr<ScriptClass> &scriptClass, EntityID id)
    : scriptClass(scriptClass)
{
    instance = scriptClass->Instantiate();
    constrcutor = ScriptEngine::GetEntityClass().GetMethod(".ctor", 1);
    onCreateMethod = scriptClass->GetMethod("OnCreate", 0);
    onUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

    void *param = &id;
    scriptClass->InvokeMethod(instance, constrcutor, &param);
}
void ScriptInstance::InvokeOnCreate()
{
    scriptClass->InvokeMethod(instance, onCreateMethod, nullptr);
}

void ScriptInstance::InvokeOnUpdate(float timestep)
{
    void *param = &timestep;
    scriptClass->InvokeMethod(instance, onUpdateMethod, &param);
}

MonoDomain *ScriptEngine::rootDomain = nullptr;
MonoDomain *ScriptEngine::appDomain = nullptr;
MonoAssembly *ScriptEngine::coreAssembly = nullptr;
MonoImage *ScriptEngine::coreAssemblyImage = nullptr;
MonoAssembly *ScriptEngine::appAssembly = nullptr;
MonoImage *ScriptEngine::appAssemblyImage = nullptr;
ScriptClass ScriptEngine::entityClass;
World *ScriptEngine::worldContext;
std::unordered_map<std::string, std::shared_ptr<ScriptClass>> ScriptEngine::entityClasses;
std::unordered_map<EntityID, std::unordered_map<std::string, std::shared_ptr<ScriptInstance>>> ScriptEngine::entityInstances;
}