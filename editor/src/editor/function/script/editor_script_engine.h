#pragma once
#include "function/script/script_engine.h"
#include "platform/filesystem/filesystem.h"
#include <filewatch.hpp>

namespace Zafkiel
{

class ScriptClass;
class ScriptInstance;
class ScriptField;

class EditorScriptEngine final : public ScriptEngine
{
  public:
    EditorScriptEngine();
    ~EditorScriptEngine();

    static void Init()
    {
        instance = new EditorScriptEngine;
        ScriptEngine::instancePtr = instance;
    }

    static void Destroy()
    {
        delete instance;
        instance = nullptr;
        ScriptEngine::instancePtr = nullptr;
    }

    static const EditorScriptEngine& Instance()
    {
        return *instance;
    }

    static void PrintAssemblyTypes(MonoImage *image) { instance->PrintAssemblyTypesImpl(image); }

    static void WatchScriptFiles(const Path &scriptDir) { instance->WatchScriptFilesImpl(scriptDir); }
    static void CompileScripts() { instance->CompileScriptsImpl(); }

    static void CreateEditorDomain() { instance->CreateEditorDomainImpl(); }
    static void CreateRuntimeDomain() { instance->CreateRuntimeDomainImpl(); }

    static void SwitchToEditor() { instance->SwitchToEditorImpl(); }
    static void SwitchToRuntime() { instance->SwitchToRuntimeImpl(); }

    static void UnloadEditorDomain() { instance->UnloadEditorDomainImpl(); }
    static void UnloadRuntimeDomain() { instance->UnloadRuntimeDomainImpl(); }

    static void ReloadEditorDomain() { instance->ReloadEditorDomainImpl(); }

    static void LoadEditorCoreAssembly() { instance->LoadEditorCoreAssemblyImpl(); }
    static void LoadRuntimeCoreAssembly() { instance->LoadRuntimeCoreAssemblyImpl(); }
    static void LoadEditorAppAssembly() { instance->LoadEditorAppAssemblyImpl(); }
    static void LoadRuntimeAppAssembly() { instance->LoadRuntimeAppAssembly(); }
  
    static void OnRuntimeInit() { instance->OnRuntimeInitImpl(); }
    static void OnRuntimeStart() { instance->OnRuntimeStartImpl(); }
    static void OnRuntimeUpdate(float timestep) { instance->OnRuntimeUpdateImpl(timestep); }
    static void OnRuntimeStop() { instance->OnRuntimeStopImpl(); }
    
    static bool IsRuntime() { return instance->isRuntime; }

    friend class ScriptClass;
    friend class ScriptField;
    friend class ScriptInstance;

    bool isRuntime = false;

  private:
    inline static EditorScriptEngine *instance = nullptr;

    virtual bool HasScriptInstanceImpl(UUID uuid, const std::string &scriptName) const override;
    virtual Ref<ScriptInstance> GetScriptInstanceImpl(UUID uuid, const std::string &scriptName) const override;
    virtual Ref<ScriptInstance> AddScriptInstanceImpl(UUID uuid, const std::string &scriptName) override;
    virtual void RemoveScriptInstanceImpl(UUID uuid, const std::string &scriptName) override;
    virtual const ScriptInstanceMap &GetScriptInstancesImpl(UUID uuid) const override;
    virtual ScriptInstanceMap &GetScriptInstancesImpl(UUID uuid) override;
    virtual bool HasScriptImpl(const std::string &name) const override;

    void PrintAssemblyTypesImpl(MonoImage *image);

    void WatchScriptFilesImpl(const Path &scriptDir);
    void CompileScriptsImpl();

    void CreateEditorDomainImpl();
    void CreateRuntimeDomainImpl();

    void SwitchToEditorImpl();
    void SwitchToRuntimeImpl();

    void UnloadEditorDomainImpl();
    void UnloadRuntimeDomainImpl();

    void ReloadEditorDomainImpl();

    void LoadEditorCoreAssemblyImpl();
    void LoadRuntimeCoreAssemblyImpl();
    void LoadEditorAppAssemblyImpl();
    void LoadRuntimeAppAssemblyImpl();

    void OnRuntimeInitImpl();
    void OnRuntimeStartImpl();
    void OnRuntimeUpdateImpl(float timestep);
    void OnRuntimeStopImpl();

    ScriptClassMap &GetScriptClasses() { return GetActiveDomain()->GetScriptClasses(); }
    const ScriptClassMap &GetScriptClasses() const { return GetActiveDomain()->GetScriptClasses(); }

    EntityMap &GetEntities() { return GetActiveDomain()->GetEntities(); }
    const EntityMap &GetEntities() const { return GetActiveDomain()->GetEntities(); }


    Ref<ScriptDomain> GetActiveDomain() const;
    bool scriptReloadPending = false;
    int assemblyIndex = 0;

    MonoDomain *rootDomain;

    Ref<ScriptDomain> editorDomain = nullptr;
    Ref<ScriptDomain> runtimeDomain = nullptr;

    std::unique_ptr<filewatch::FileWatch<std::filesystem::path>> scriptFileWatcher;
};

}