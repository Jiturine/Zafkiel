#pragma once
#include "entt/entity/fwd.hpp"
#include <memory>
#include <mono/jit/jit.h>
#include <mono/metadata/image.h>
#include "Platform/Filesystem/Filesystem.h"
#include <mono/metadata/object-forward.h>
#include <unordered_map>
#include "Function/Scene/World.h"

namespace Zafkiel
{

class ScriptClass;
class ScriptInstance;
class ScriptField;

using ScriptInstanceMap = std::unordered_map<std::string, Ref<ScriptInstance>>;
using EntityMap = std::unordered_map<UUID, ScriptInstanceMap>;
using ScriptClassMap = std::unordered_map<std::string, Ref<ScriptClass>>;

// 分 Editor 和 Runtime
class ScriptEngine
{
  public:
    virtual ~ScriptEngine() = default;

    static ScriptInstanceMap &GetScriptInstances(UUID uuid) { return instancePtr->GetScriptInstancesImpl(uuid); }
    static bool HasScriptInstance(UUID uuid, const std::string &scriptName) { return instancePtr->HasScriptInstanceImpl(uuid, scriptName); }
    static Ref<ScriptInstance> GetScriptInstance(UUID uuid, const std::string &scriptName) { return instancePtr->GetScriptInstanceImpl(uuid, scriptName); }
    static Ref<ScriptInstance> AddScriptInstance(UUID uuid, const std::string &scriptName) { return instancePtr->AddScriptInstanceImpl(uuid, scriptName); }
    static void RemoveScriptInstance(UUID uuid, const std::string &scriptName) { instancePtr->RemoveScriptInstanceImpl(uuid, scriptName); }
    static bool HasScript(const std::string &name) { return instancePtr->HasScriptImpl(name); }
    
  protected:
    virtual const ScriptInstanceMap &GetScriptInstancesImpl(UUID uuid) const = 0;
    virtual ScriptInstanceMap &GetScriptInstancesImpl(UUID uuid) = 0;
    virtual bool HasScriptInstanceImpl(UUID uuid, const std::string &scriptName) const = 0;
    virtual Ref<ScriptInstance> GetScriptInstanceImpl(UUID uuid, const std::string &scriptName) const = 0;
    virtual Ref<ScriptInstance> AddScriptInstanceImpl(UUID uuid, const std::string &scriptName) = 0;
    virtual void RemoveScriptInstanceImpl(UUID uuid, const std::string &scriptName) = 0;
    virtual bool HasScriptImpl(const std::string &name) const = 0;
    
    inline static ScriptEngine *instancePtr;
};

class ScriptDomain : public RefCounted
{
  public:
    ScriptDomain(const std::string &name);
    ~ScriptDomain();
    void LoadCoreAssembly(const Path &path);
    void LoadAppAssembly(const Path &path);
    void SetCurrent();
    Ref<ScriptInstance> InstantiateScriptClass(const Ref<ScriptClass> &scriptClass, UUID uuid);
    MonoDomain *GetHandle() const { return handle; }
    MonoAssembly *GetCoreAssembly() const { return coreAssembly; }
    MonoImage *GetCoreAssemblyImage() const { return coreAssemblyImage; }
    MonoAssembly *GetAppAssembly() const { return appAssembly; }
    MonoImage *GetAppAssemblyImage() const { return appAssemblyImage; }
    ScriptClassMap &GetScriptClasses() { return scriptClasses; }
    EntityMap &GetEntities() { return entities; }
  private:
    Ref<ScriptClass> RegisterCoreClass(const std::string &namespaceStr, const std::string &nameStr);
    void RegisterAppClass(const std::string &namespaceStr, const std::string &nameStr);
    MonoDomain *handle;
    std::string name;
    MonoAssembly *coreAssembly;
    MonoImage *coreAssemblyImage;
    MonoAssembly *appAssembly;
    MonoImage *appAssemblyImage;
    Ref<ScriptClass> entityClass;
    ScriptClassMap scriptClasses;
    EntityMap entities;
};

enum class ScriptFieldType
{
    Unknown,
    Bool, Char,
    Int16, Int32, Int64,
    UInt8, UInt16, UInt32, UInt64,
    Float, Double,
    Vector2, Vector3, Vector4,
    Entity
};

struct ScriptField
{
    ScriptFieldType type;
    MonoClassField *handle;
};

struct ScriptMethod
{
    MonoMethod *handle;
    bool isStatic;
};

class ScriptClass : public RefCounted
{
  public:
    friend class ScriptInstance;
    friend class ScriptDomain;

    MonoObject *Instantiate() const;
    ScriptField GetField(const std::string &name) const;
    ScriptMethod GetMethod(const std::string &name) const;
    MonoObject *InvokeStaticMethod(const std::string &name, void **params) const;
    std::unordered_map<std::string, ScriptField> GetFields() const { return fields; }
    std::unordered_map<std::string, ScriptMethod> GetMethods() const { return methods; }
    MonoClass *GetHandle() const { return monoClass; }
  private:
    template <typename T, typename... Args>
    friend Ref<T> CreateRef(Args &&...args);

    ScriptClass(MonoClass *monoClass, const std::string &classNamespace, const std::string &className);
    std::string classNamespace;
    std::string className;
    std::unordered_map<std::string, ScriptField> fields;
    std::unordered_map<std::string, ScriptMethod> methods;
    MonoClass *monoClass = nullptr;
};

class ScriptInstance : public RefCounted
{
  public:
    ScriptInstance(MonoObject *instance, const Ref<ScriptClass> &scriptClass);
    MonoObject *InvokeMethod(const std::string &name, void **params);
    MonoObject *InvokeMethod(ScriptMethod method, void **params);
    void TryInvokeOnCreate();
    void TryInvokeOnUpdate(float timestep);
    void TryInvokeOnDestroy();

    Ref<ScriptClass> GetScriptClass() const { return scriptClass; }

    template <typename T>
    T GetFieldValue(const std::string &name)
    {
        auto it = scriptClass->fields.find(name);
        if (it == scriptClass->fields.end())
            return T{};
        ScriptField &field = it->second;
        mono_field_get_value(instance, field.handle, fieldValueBuffer);
        return *(T *)fieldValueBuffer;
    }

    template <typename T>
    void SetFieldValue(const std::string &name, const T &value)
    {
        auto it = scriptClass->fields.find(name);
        if (it == scriptClass->fields.end())
            return;
        ScriptField &field = it->second;
        mono_field_set_value(instance, field.handle, (void *)&value);
    }

    MonoObject *GetHandle() const { return instance; }

  private:
    Ref<ScriptClass> scriptClass;
    MonoObject *instance;
    inline static char fieldValueBuffer[16];
};

}