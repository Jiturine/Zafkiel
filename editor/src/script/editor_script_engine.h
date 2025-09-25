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
    EditorScriptEngine(const Path &coreAssemblyPath);
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

    EntityInstanceMap &GetEntityInstances() { return GetActiveDomain()->GetEntityInstances(); }
    const EntityInstanceMap &GetEntityInstances() const { return GetActiveDomain()->GetEntityInstances(); }

    static void OnScriptsChange(const std::filesystem::path &path, const filewatch::Event change_type);

    virtual bool HasEntityScriptInstance(UUID uuid, const std::string &scriptName) const override
    {
        auto &entityInstances = GetEntityInstances();
        auto it = entityInstances.find(uuid);
        return it != entityInstances.end() && it->second.contains(scriptName);
    }

    virtual Ref<ScriptInstance> GetEntityScriptInstance(UUID uuid, const std::string &scriptName) const override
    {
        auto &entityInstances = GetEntityInstances();
        if (auto entity = entityInstances.find(uuid); entity != entityInstances.end())
            if (auto it = entity->second.find(scriptName); it != entity->second.end())
                return it->second;
        Log::CoreError("entity script doesn't exist: {} {}", (uint64_t)uuid, scriptName);
        return nullptr;
    }

    virtual Ref<ScriptInstance> AddEntityScriptInstance(UUID uuid, const std::string &scriptName) override
    {
        auto &scriptClasses = GetScriptClasses();
        auto it = scriptClasses.find(scriptName);
        if (it == scriptClasses.end())
        {
            Log::CoreError("Cannot Find Script Class: {}", scriptName);
            return nullptr;
        }
        auto instance = GetActiveDomain()->InstantiateScriptClass(it->second, uuid);
        GetEntityInstances()[uuid][scriptName] = instance;
        return instance;
    }

    virtual void RemoveEntityScriptInstance(UUID uuid, const std::string &scriptName) override
    {
        auto entityInstances = GetEntityInstances();
        if (auto entity = entityInstances.find(uuid); entity != entityInstances.end())
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