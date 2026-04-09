#include "editor/Function/Script/EditorScriptEngine.h"
#include "Function/Scene/Components.h"
#include "Function/Scene/Scene.h"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include "mono/metadata/metadata.h"
#include "mono/metadata/object.h"
#include "mono/metadata/mono-gc.h"
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/attrdefs.h>
#include "Function/Script/ScriptGlue.h"
#include <filewatch.hpp>
#include "Core/Meta/Serializer/YamlSerializer.h"
#include "Core/Application/Application.h"
#include "editor/Project/ProjectManager.h"

namespace Zafkiel
{

static std::string MonoStringToCppString(MonoString *string)
{
    char *cStr = mono_string_to_utf8(string);
    std::string res(cStr);
    mono_free(cStr);
    return res;
}

EditorScriptEngine::EditorScriptEngine()
{
    std::vector<const char *> argv = {
        "--debugger-agent=transport=dt_socket,address=localhost:55555,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
        "--soft-breakpoints"};
    mono_jit_parse_options(argv.size(), (char **)argv.data());
    mono_debug_init(MONO_DEBUG_FORMAT_MONO);

    rootDomain = mono_jit_init("ZafkielJITRuntime");
    assert(rootDomain);

    mono_domain_set(rootDomain, true);
}

EditorScriptEngine::~EditorScriptEngine()
{
    if (rootDomain)
    {
        mono_domain_set(rootDomain, false);
    }
    editorDomain = nullptr;
    runtimeDomain = nullptr;
    rootDomain = nullptr;
}

void EditorScriptEngine::CreateEditorDomainImpl()
{
    editorDomain = CreateRef<ScriptDomain>("Editor Domain");
}
void EditorScriptEngine::CreateRuntimeDomainImpl()
{
    runtimeDomain = CreateRef<ScriptDomain>("Runtime Domain");
    mono_debug_domain_create(runtimeDomain->GetHandle());
}

bool EditorScriptEngine::HasScriptInstanceImpl(UUID uuid, const std::string &scriptName) const
{
    auto &entities = GetEntities();
    auto it = entities.find(uuid);
    return it != entities.end() && it->second.contains(scriptName);
}

Ref<ScriptInstance> EditorScriptEngine::GetScriptInstanceImpl(UUID uuid, const std::string &scriptName) const
{
    auto &entities = GetEntities();
    if (auto entity = entities.find(uuid); entity != entities.end())
        if (auto it = entity->second.find(scriptName); it != entity->second.end())
            return it->second;
    Log::Error("entity script doesn't exist: {} {}", (uint64_t)uuid, scriptName);
    return nullptr;
}

Ref<ScriptInstance> EditorScriptEngine::AddScriptInstanceImpl(UUID uuid, const std::string &scriptName)
{
    auto &scriptClasses = GetScriptClasses();
    auto it = scriptClasses.find(scriptName);
    if (it == scriptClasses.end())
    {
        Log::Error("Cannot Find Script Class: {}", scriptName);
        return nullptr;
    }
    auto instance = GetActiveDomain()->InstantiateScriptClass(it->second, uuid);
    GetEntities()[uuid][scriptName] = instance;
    return instance;
}

void EditorScriptEngine::RemoveScriptInstanceImpl(UUID uuid, const std::string &scriptName)
{
    auto &entities = GetEntities();
    if (auto entity = entities.find(uuid); entity != entities.end())
        entity->second.erase(scriptName);
    else
        Log::Error("Entity Instance {} - {} doesn't exist!", (uint64_t)uuid, scriptName);
}

Ref<ScriptDomain> EditorScriptEngine::GetActiveDomain() const
{
    return isRuntime ? runtimeDomain : editorDomain;
}

const ScriptInstanceMap &EditorScriptEngine::GetScriptInstancesImpl(UUID uuid) const
{
    auto &entities = GetEntities();
    if (auto it = entities.find(uuid); it != entities.end())
        return it->second;
    else
    {
        Log::Error("Cannot find Script Instances of Entity : {}", (uint64_t)uuid);
        static const ScriptInstanceMap empty;
        return empty;
    }
}

ScriptInstanceMap &EditorScriptEngine::GetScriptInstancesImpl(UUID uuid)
{
    auto &entities = GetEntities();
    if (auto it = entities.find(uuid); it != entities.end())
        return it->second;
    else
    {
        Log::Error("Cannot find Script Instances of Entity : {}", (uint64_t)uuid);
        static ScriptInstanceMap empty;
        return empty;
    }
}

bool EditorScriptEngine::HasScriptImpl(const std::string &name) const
{
    return GetScriptClasses().contains(name);
}

void EditorScriptEngine::SwitchToEditorImpl()
{
    editorDomain->SetCurrent();
    isRuntime = false;
}
void EditorScriptEngine::SwitchToRuntimeImpl()
{
    runtimeDomain->SetCurrent();
    isRuntime = true;
}

void EditorScriptEngine::UnloadEditorDomainImpl()
{
    if (!editorDomain)
        return;
    mono_domain_set(rootDomain, false);
    editorDomain = nullptr;
}
void EditorScriptEngine::UnloadRuntimeDomainImpl()
{
    if (!runtimeDomain)
        return;
    mono_domain_set(rootDomain, false);
    runtimeDomain = nullptr;
}

void EditorScriptEngine::ReloadEditorDomainImpl()
{
    bool currentIsRuntime = isRuntime;
    SwitchToEditor();
    // 没有专门序列化脚本的，因此序列化整个场景，这时需要更换scene
    // auto data = Editor::GetEditorScene()->Serialize();
    UnloadEditorDomain();
    CompileScripts();
    CreateEditorDomain();
    LoadEditorCoreAssembly();
    LoadEditorAppAssembly();
    SwitchToEditor();

/*     Ref<Scene> newScene = CreateRef<Scene>(data);
    if (!currentIsRuntime) Engine::SetActiveScene(newScene);
    Editor::SetEditorScene(newScene) */;

    if (currentIsRuntime) SwitchToRuntime();
    else SwitchToEditor();
}

void EditorScriptEngine::LoadEditorCoreAssemblyImpl()
{
    editorDomain->LoadCoreAssembly("ScriptCore.dll");
    // debug
    PrintAssemblyTypes(editorDomain->GetCoreAssemblyImage());
}
void EditorScriptEngine::LoadRuntimeCoreAssemblyImpl()
{
    runtimeDomain->LoadCoreAssembly("ScriptCore.dll");

    Path pdbPath = "ScriptCore.dll";
    pdbPath.replace_extension(".pdb");
    if (std::filesystem::exists(pdbPath))
    {
        ScopedBuffer pdbFileData = FileSystem::ReadBytes(pdbPath);
        mono_debug_open_image_from_memory(runtimeDomain->GetCoreAssemblyImage(), pdbFileData.Data<uint8>(), pdbFileData.Size<uint8>());
    }
}

void EditorScriptEngine::LoadEditorAppAssemblyImpl()
{
    auto assemblyPath = ProjectManager::GetCurrentProject()->GetLibraryDirectory() / std::format("AppAssembly_{}.dll", assemblyIndex - 1);
    editorDomain->LoadAppAssembly(assemblyPath);
    // debug
    PrintAssemblyTypes(editorDomain->GetAppAssemblyImage());
}
void EditorScriptEngine::LoadRuntimeAppAssemblyImpl()
{
    auto assemblyPath = ProjectManager::GetCurrentProject()->GetLibraryDirectory() / std::format("AppAssembly_{}.dll", assemblyIndex - 1);
    runtimeDomain->LoadAppAssembly(assemblyPath);

    Path pdbPath = assemblyPath;
    pdbPath.replace_extension(".pdb");
    if (std::filesystem::exists(pdbPath))
    {
        ScopedBuffer pdbFileData = FileSystem::ReadBytes(pdbPath);
        mono_debug_open_image_from_memory(runtimeDomain->GetAppAssemblyImage(), pdbFileData.Data<uint8>(), pdbFileData.Size<uint8>());
    }
}

void EditorScriptEngine::PrintAssemblyTypesImpl(MonoImage *image)
{
    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    size_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    for (size_t i = 0; i < numTypes; i++)
    {
        uint32 cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char *namespaceStr = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char *nameStr = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        Log::Debug("[[APP ASSEMBLY]]: {}.{}", namespaceStr, nameStr);
    }
}

void EditorScriptEngine::WatchScriptFilesImpl(const Path &scriptDir)
{
    scriptFileWatcher = std::make_unique<filewatch::FileWatch<std::filesystem::path>>(
        scriptDir, [this](const std::filesystem::path &file, const filewatch::Event event_type) {
            if (!scriptReloadPending && event_type == filewatch::Event::modified)
            {
                scriptReloadPending = true;
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(100ms);

                Application::Instance().SubmitToMainThread([&]() {
                    ReloadEditorDomain();
                });
            } });
}

void EditorScriptEngine::CompileScriptsImpl()
{
    const Path &libraryDir = ProjectManager::GetCurrentProject()->GetLibraryDirectory();
    Path appAssemblyPath = libraryDir / std::format("AppAssembly_{}.dll", assemblyIndex++);
    if (!std::filesystem::exists(libraryDir))
    {
        std::filesystem::create_directory(libraryDir);
    }
    Path sourcePath = ProjectManager::GetCurrentProject()->GetAssetDirectory() / "scripts" / "*.cs";
    std::string cmd = std::format("csc -target:library -debug:portable -r:ScriptCore.dll -out:{} {}", appAssemblyPath.string(), sourcePath.string());
    Log::Info("Compiling scripts: {}", cmd);
    std::system(cmd.c_str());

    scriptReloadPending = false;
}

void EditorScriptEngine::OnRuntimeInitImpl()
{
    CreateRuntimeDomain();
    SwitchToRuntime();
    LoadRuntimeCoreAssembly();
    LoadRuntimeAppAssembly();
}
void EditorScriptEngine::OnRuntimeStartImpl()
{
    for (auto &[uuid, entity] : runtimeDomain->GetEntities())
    {
        for (auto &[scriptName, instance] : entity)
        {
            instance->TryInvokeOnCreate();
        }
    }
}

void EditorScriptEngine::OnRuntimeUpdateImpl(float timestep)
{
    for (auto &[uuid, entity] : runtimeDomain->GetEntities())
    {
        for (auto &[scriptName, instance] : entity)
        {
            instance->TryInvokeOnUpdate(timestep);
        }
    }
}

void EditorScriptEngine::OnRuntimeStopImpl()
{
    for (auto &[uuid, entity] : runtimeDomain->GetEntities())
    {
        for (auto &[scriptName, instance] : entity)
        {
            instance->TryInvokeOnDestroy();
        }
    }
    UnloadRuntimeDomain();
    SwitchToEditor();
}

}