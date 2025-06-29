#pragma once
#include "entt/entity/fwd.hpp"
#include <memory>
#include <mono/jit/jit.h>
#include <mono/metadata/image.h>
#include <filesystem>
#include <mono/metadata/object-forward.h>
#include <unordered_map>
#include <ECS/world.h>

namespace Zafkiel
{

class ScriptClass;
class ScriptInstance;

class ScriptEngine
{
  public:
    static void Init();
    static void Shutdown();
    static void LoadCoreAssembly(const std::filesystem::path &assemblyPath);
    static void LoadAppAssembly(const std::filesystem::path &assemblyPath);
    static void OnRuntimeStart(World &world);
    static void OnRuntimeStop();
    static MonoObject *InstantiateClass(MonoClass *monoClass);
    static MonoImage *GetCoreAssemblyImage() { return coreAssemblyImage; }
    static MonoImage *GetAppAssemblyImage() { return appAssemblyImage; }
    static ScriptClass &GetEntityClass() { return entityClass; }
    static World *GetWorldContext() { return worldContext; }
  private:
    static void PrintCoreAssemblyTypes();
    static void PrintAppAssemblyTypes();
    static void LoadAssemblyClasses();
    static MonoDomain *rootDomain;
    static MonoDomain *appDomain;
    static MonoAssembly *coreAssembly;
    static MonoImage *coreAssemblyImage;
    static MonoAssembly *appAssembly;
    static MonoImage *appAssemblyImage;
    static ScriptClass entityClass;
    static World *worldContext;
    static std::unordered_map<std::string, std::shared_ptr<ScriptClass>> entityClasses;
    static std::unordered_map<EntityID, std::unordered_map<std::string, std::shared_ptr<ScriptInstance>>> entityInstances;
};

class ScriptClass
{
  public:
    ScriptClass() = default;
    ScriptClass(const std::string &classNamespace, const std::string &className, bool isCore);
    MonoObject *Instantiate();
    MonoMethod *GetMethod(const std::string &name, int paramCount);
    MonoObject *InvokeMethod(MonoObject *instance, MonoMethod *method, void **params);
  private:
    std::string classNamespace;
    std::string className;
    MonoClass *monoClass = nullptr;
};

class ScriptInstance
{
  public:
    ScriptInstance(const std::shared_ptr<ScriptClass> &scriptClass, EntityID id);
    void InvokeOnCreate();
    void InvokeOnUpdate(float timestep);
  private:
    std::shared_ptr<ScriptClass> scriptClass;
    MonoObject *instance;
    MonoMethod *constrcutor;
    MonoMethod *onCreateMethod;
    MonoMethod *onUpdateMethod;
};

}