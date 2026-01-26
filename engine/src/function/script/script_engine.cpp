#include "function/script/script_engine.h"
#include "function/scene/components.h"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/attrdefs.h"
#include "mono/metadata/class.h"
#include "mono/metadata/mono-gc.h"
#include "function/script/script_glue.h"
#include <cstring>
#include <mono/metadata/assembly.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/object.h>
#include "platform/filesystem/filesystem.h"
#include "function/scene/scene.h"

namespace Zafkiel
{
static std::unordered_map<std::string, ScriptFieldType> stringToScriptFieldType = {
    {"System.Boolean", ScriptFieldType::Bool},
    {"System.Char", ScriptFieldType::Char},
    {"System.Single", ScriptFieldType::Float},
    {"System.Double", ScriptFieldType::Double},
    {"System.Int16", ScriptFieldType::Int16},
    {"System.Int32", ScriptFieldType::Int32},
    {"System.Int64", ScriptFieldType::Int64},
    {"System.Byte", ScriptFieldType::UInt8},
    {"System.UInt16", ScriptFieldType::UInt16},
    {"System.UInt32", ScriptFieldType::UInt32},
    {"System.UInt64", ScriptFieldType::UInt64},
    {"Zafkiel.Vector2", ScriptFieldType::Vector2},
    {"Zafkiel.Vector3", ScriptFieldType::Vector3},
    {"Zafkiel.Vector4", ScriptFieldType::Vector4},
    {"Zafkiel.Entity", ScriptFieldType::Entity}};

static std::string MonoStringToCppString(MonoString *string)
{
    char *cStr = mono_string_to_utf8(string);
    std::string res(cStr);
    mono_free(cStr);
    return res;
}

Ref<ScriptInstance> ScriptDomain::InstantiateScriptClass(const Ref<ScriptClass> &scriptClass, UUID uuid)
{
    auto rawInstance = mono_object_new(handle, scriptClass->monoClass);
    if (!rawInstance)
    {
        Log::Error("Failed to Instantiate Class: {}.{}", scriptClass->classNamespace, scriptClass->className);
        return nullptr;
    }
    mono_runtime_object_init(rawInstance);
    auto constructor = entityClass->GetMethod(".ctor");
    auto instance = CreateRef<ScriptInstance>(rawInstance, scriptClass);
    void *params = &uuid;
    instance->InvokeMethod(constructor, &params);
    return instance;
}

ScriptDomain::ScriptDomain(const std::string &name)
    : name(name)
{
    handle = mono_domain_create_appdomain((char *)"Editor Domain", nullptr);
    if (!handle)
    {
        Log::Error("Failed to Create Domain!");
        return;
    }
    mono_domain_set(handle, false);
}

ScriptDomain::~ScriptDomain()
{
    scriptClasses.clear();
    entities.clear();
    coreAssembly = nullptr;
    coreAssemblyImage = nullptr;
    appAssembly = nullptr;
    appAssemblyImage = nullptr;
    entityClass = nullptr;
#ifndef NDEBUG
    // mono_domain_unload(handle);
#endif
    // mono_gc_collect(mono_gc_max_generation());
    handle = nullptr;
}

void ScriptDomain::SetCurrent()
{
    mono_domain_set(handle, false);
}
void ScriptDomain::LoadCoreAssembly(const Path &path)
{
    mono_domain_set(handle, false);
    coreAssembly = mono_domain_assembly_open(handle, path.string().c_str());
    if (!coreAssembly)
    {
        Log::Error("Failed to Load Core Assembly: {}", path.string());
    }
    coreAssemblyImage = mono_assembly_get_image(coreAssembly);
    if (!coreAssemblyImage)
    {
        Log::Error("Failed to Load Core Assembly Image: {}", path.string());
    }

    entityClass = RegisterCoreClass("Zafkiel", "Entity");

    ScriptGlue::AddInternalCalls();
}

void ScriptDomain::LoadAppAssembly(const Path &path)
{
    if (!coreAssembly)
    {
        Log::Error("Need to Load Core Assembly first!");
        return;
    }
    mono_domain_set(handle, false);
    appAssembly = mono_domain_assembly_open(handle, path.string().c_str());
    if (!appAssembly)
    {
        Log::Error("Failed to App Assembly: {}", path.string());
    }
    appAssemblyImage = mono_assembly_get_image(appAssembly);
    if (!appAssemblyImage)
    {
        Log::Error("Failed to App Assembly Image: {}", path.string());
    }

    scriptClasses.clear();
    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(appAssemblyImage, MONO_TABLE_TYPEDEF);
    size_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    for (size_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        std::string namespaceStr = mono_metadata_string_heap(appAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
        std::string nameStr = mono_metadata_string_heap(appAssemblyImage, cols[MONO_TYPEDEF_NAME]);

        RegisterAppClass(namespaceStr, nameStr);
    }
}

Ref<ScriptClass> ScriptDomain::RegisterCoreClass(const std::string &namespaceStr, const std::string &nameStr)
{
    auto monoClass = mono_class_from_name(coreAssemblyImage, namespaceStr.c_str(), nameStr.c_str());
    return CreateRef<ScriptClass>(monoClass, namespaceStr, nameStr);
}

void ScriptDomain::RegisterAppClass(const std::string &namespaceStr, const std::string &nameStr)
{
    auto monoClass = mono_class_from_name(appAssemblyImage, namespaceStr.c_str(), nameStr.c_str());
    bool isEntity = mono_class_is_subclass_of(monoClass, entityClass->GetHandle(), false);
    if (isEntity)
    {
        std::string fullName = !namespaceStr.empty() ? std::format("{}.{}", namespaceStr, nameStr) : nameStr;
        scriptClasses[fullName] = CreateRef<ScriptClass>(monoClass, namespaceStr, nameStr);
    }
}

MonoObject *ScriptClass::InvokeStaticMethod(const std::string &name, void **params) const
{
    auto it = methods.find(name);
    if (it == methods.end())
    {
        Log::Error("Cannot Find Method: {}", name);
        return nullptr;
    }
    ScriptMethod method = it->second;
    if (!method.isStatic)
    {
        Log::Error("Method {} isn't Static!", name);
        return nullptr;
    }
    MonoObject *exc = nullptr;
    MonoObject *ret = mono_runtime_invoke(method.handle, nullptr, params, &exc);
    if (exc)
    {
        MonoString *excMonoStr = mono_object_to_string(exc, nullptr);
        std::string excStr = MonoStringToCppString(excMonoStr);
        Log::Error("Invoke Static Method Exception: {}", excStr);
        return nullptr;
    }
    return ret;
}

ScriptClass::ScriptClass(MonoClass *monoClass, const std::string &classNamespace, const std::string &className)
    : monoClass(monoClass), classNamespace(classNamespace), className(className)
{
    void *iterator = nullptr;
    while (auto field = mono_class_get_fields(monoClass, &iterator))
    {
        std::string fieldName = mono_field_get_name(field);
        auto fieldType = mono_field_get_type(field);
        std::string typeName = mono_type_get_name(fieldType);
        ScriptFieldType type = stringToScriptFieldType.contains(typeName) ? stringToScriptFieldType[typeName] : ScriptFieldType::Unknown;
        fields[fieldName] = ScriptField{type, field};
    }
    iterator = nullptr;
    while (auto method = mono_class_get_methods(monoClass, &iterator))
    {
        std::string methodName = mono_method_get_name(method);
        uint32_t flags;
        mono_method_get_flags(method, &flags);
        bool isStatic = flags & MONO_METHOD_ATTR_STATIC;
        methods[methodName] = ScriptMethod{method, isStatic};
    }
}

ScriptMethod ScriptClass::GetMethod(const std::string &name) const
{
    auto it = methods.find(name);
    if (it == methods.end())
    {
        Log::Error("Cannot Find Script Method: {}", name);
        return ScriptMethod{};
    }
    return it->second;
}

ScriptField ScriptClass::GetField(const std::string &name) const
{
    auto it = fields.find(name);
    if (it == fields.end())
    {
        Log::Error("Cannot Find Script Field: {}", name);
        return ScriptField{};
    }
    return it->second;
}

MonoObject *ScriptInstance::InvokeMethod(const std::string &name, void **params)
{
    auto it = scriptClass->methods.find(name);
    if (it == scriptClass->methods.end())
    {
        Log::Error("Cannot Find Method: {}", name);
        return nullptr;
    }
    ScriptMethod method = it->second;
    if (method.isStatic)
    {
        Log::Error("Method {} is Static!", name);
        return nullptr;
    }
    MonoObject *exc = nullptr;
    MonoObject *ret = mono_runtime_invoke(method.handle, instance, params, &exc);
    if (exc)
    {
        MonoString *excMonoStr = mono_object_to_string(exc, nullptr);
        std::string excStr = MonoStringToCppString(excMonoStr);
        Log::Error("Invoke Method Exception: {}", excStr);
        return nullptr;
    }
    return ret;
}

MonoObject *ScriptInstance::InvokeMethod(ScriptMethod method, void **params)
{
    if (method.isStatic)
    {
        Log::Error("Method is Static!");
        return nullptr;
    }
    MonoObject *exc = nullptr;
    MonoObject *ret = mono_runtime_invoke(method.handle, instance, params, &exc);
    if (exc)
    {
        MonoString *excMonoStr = mono_object_to_string(exc, nullptr);
        std::string excStr = MonoStringToCppString(excMonoStr);
        Log::Error("Invoke Method Exception: {}", excStr);
        return nullptr;
    }
    return ret;
}

void ScriptInstance::TryInvokeOnCreate()
{
    if (scriptClass->methods.contains("OnCreate"))
        InvokeMethod("OnCreate", nullptr);
}

void ScriptInstance::TryInvokeOnUpdate(float timestep)
{
    if (scriptClass->methods.contains("OnUpdate"))
    {
        void *param = &timestep;
        InvokeMethod("OnUpdate", &param);
    }
}

void ScriptInstance::TryInvokeOnDestroy()
{
    if (scriptClass->methods.contains("OnDestroy"))
        InvokeMethod("OnDestroy", nullptr);
}

ScriptInstance::ScriptInstance(MonoObject *instance, const Ref<ScriptClass> &scriptClass)
    : instance(instance), scriptClass(scriptClass) {}
}
