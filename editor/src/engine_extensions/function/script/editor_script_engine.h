#pragma once
#include "function/script/script_engine.h"
#include "platform/filesystem/filesystem.h"
#include <filewatch.hpp>

namespace Zafkiel
{

class ScriptClass;
class ScriptInstance;
class ScriptField;

class EditorScriptEngine : public ScriptEngine
{
  public:
    EditorScriptEngine();
    ~EditorScriptEngine();

    // debug
    void PrintAssemblyTypes(MonoImage *image);

    void WatchScriptFiles(const Path &scriptDir);
    void CompileScripts();

    void CreateEditorDomain();
    void CreateRuntimeDomain();

    void SwitchToEditor();
    void SwitchToRuntime();

    void UnloadEditorDomain();
    void UnloadRuntimeDomain();

    void ReloadEditorDomain();

    void LoadEditorCoreAssembly();
    void LoadRuntimeCoreAssembly();
    void LoadEditorAppAssembly();
    void LoadRuntimeAppAssembly();

    void OnRuntimeInit();
    void OnRuntimeStart();
    void OnRuntimeUpdate(float timestep);
    void OnRuntimeStop();

    ScriptClassMap &GetScriptClasses() { return GetActiveDomain()->GetScriptClasses(); }
    const ScriptClassMap &GetScriptClasses() const { return GetActiveDomain()->GetScriptClasses(); }

    EntityMap &GetEntities() { return GetActiveDomain()->GetEntities(); }
    const EntityMap &GetEntities() const { return GetActiveDomain()->GetEntities(); }

    virtual bool HasScriptInstance(UUID uuid, const std::string &scriptName) const override
    {
        auto &entities = GetEntities();
        auto it = entities.find(uuid);
        return it != entities.end() && it->second.contains(scriptName);
    }

    virtual Ref<ScriptInstance> GetScriptInstance(UUID uuid, const std::string &scriptName) const override
    {
        auto &entities = GetEntities();
        if (auto entity = entities.find(uuid); entity != entities.end())
            if (auto it = entity->second.find(scriptName); it != entity->second.end())
                return it->second;
        Log::CoreError("entity script doesn't exist: {} {}", (uint64_t)uuid, scriptName);
        return nullptr;
    }

    virtual Ref<ScriptInstance> AddScriptInstance(UUID uuid, const std::string &scriptName) override
    {
        auto &scriptClasses = GetScriptClasses();
        auto it = scriptClasses.find(scriptName);
        if (it == scriptClasses.end())
        {
            Log::CoreError("Cannot Find Script Class: {}", scriptName);
            return nullptr;
        }
        auto instance = GetActiveDomain()->InstantiateScriptClass(it->second, uuid);
        GetEntities()[uuid][scriptName] = instance;
        return instance;
    }

    virtual void RemoveScriptInstance(UUID uuid, const std::string &scriptName) override
    {
        auto &entities = GetEntities();
        if (auto entity = entities.find(uuid); entity != entities.end())
            entity->second.erase(scriptName);
        else
            Log::CoreError("Entity Instance {} - {} doesn't exist!", (uint64_t)uuid, scriptName);
    }
    bool HasScript(const std::string &name)
    {
        return GetScriptClasses().contains(name);
    }
    Ref<ScriptDomain> GetActiveDomain() const
    {
        return isRuntime ? runtimeDomain : editorDomain;
    }

    virtual const ScriptInstanceMap &GetScriptInstances(UUID uuid) const override
    {
        auto &entities = GetEntities();
        if (auto it = entities.find(uuid); it != entities.end())
            return it->second;
        else
        {
            Log::CoreError("Cannot find Script Instances of Entity : {}", (uint64_t)uuid);
            static const ScriptInstanceMap empty;
            return empty;
        }
    }

    virtual ScriptInstanceMap &GetScriptInstances(UUID uuid) override
    {
        auto &entities = GetEntities();
        if (auto it = entities.find(uuid); it != entities.end())
            return it->second;
        else
        {
            Log::CoreError("Cannot find Script Instances of Entity : {}", (uint64_t)uuid);
            static ScriptInstanceMap empty;
            return empty;
        }
    }

    friend class ScriptClass;
    friend class ScriptField;
    friend class ScriptInstance;

    bool isRuntime = false;
  private:
    bool scriptReloadPending = false;
    int assemblyIndex = 0;

    MonoDomain *rootDomain;

    Ref<ScriptDomain> editorDomain = nullptr;
    Ref<ScriptDomain> runtimeDomain = nullptr;

    std::unique_ptr<filewatch::FileWatch<std::filesystem::path>> scriptFileWatcher;
};

}