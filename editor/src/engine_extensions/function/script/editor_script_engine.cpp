#include "editor_script_engine.h"
#include "editor.h"
#include "function/scene/components.h"
#include "function/scene/scene.h"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include "mono/metadata/metadata.h"
#include "mono/metadata/object.h"
#include "mono/metadata/mono-gc.h"
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/attrdefs.h>
#include "function/script/script_glue.h"
#include <filewatch.hpp>

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
        mono_gc_collect(mono_gc_max_generation());
    }
    editorDomain = nullptr;
    runtimeDomain = nullptr;
    rootDomain = nullptr;
}

void EditorScriptEngine::CreateEditorDomain()
{
    editorDomain = MakeRef<ScriptDomain>("Editor Domain");
}
void EditorScriptEngine::CreateRuntimeDomain()
{
    runtimeDomain = MakeRef<ScriptDomain>("Runtime Domain");
    mono_debug_domain_create(runtimeDomain->GetHandle());
}

void EditorScriptEngine::SwitchToEditor()
{
    editorDomain->SetCurrent();
    isRuntime = false;
}
void EditorScriptEngine::SwitchToRuntime()
{
    runtimeDomain->SetCurrent();
    isRuntime = true;
}

void EditorScriptEngine::UnloadEditorDomain()
{
    if (!editorDomain)
        return;
    mono_domain_set(rootDomain, false);
    editorDomain = nullptr;
}
void EditorScriptEngine::UnloadRuntimeDomain()
{
    if (!runtimeDomain)
        return;
    mono_domain_set(rootDomain, false);
    runtimeDomain = nullptr;
}

void EditorScriptEngine::ReloadEditorDomain()
{
    bool currentIsRuntime = isRuntime;
    SwitchToEditor();
    auto data = Serialize(Editor::GetEditorScene()->GetWorld());
    UnloadEditorDomain();
    CompileScripts();
    CreateEditorDomain();
    LoadEditorCoreAssembly();
    LoadEditorAppAssembly();
    SwitchToEditor();
    Editor::GetEditorScene()->GetWorld() = Deserialize<World>(data);
    if (currentIsRuntime) SwitchToRuntime();
    else SwitchToEditor();
}

void EditorScriptEngine::LoadEditorCoreAssembly()
{
    editorDomain->LoadCoreAssembly("ScriptCore.dll");
    // debug
    PrintAssemblyTypes(editorDomain->GetCoreAssemblyImage());
}
void EditorScriptEngine::LoadRuntimeCoreAssembly()
{
    runtimeDomain->LoadCoreAssembly("ScriptCore.dll");

    Path pdbPath = "ScriptCore.dll";
    pdbPath.replace_extension(".pdb");
    if (std::filesystem::exists(pdbPath))
    {
        Buffer pdbFileData = FileSystem::ReadBytes(pdbPath);
        mono_debug_open_image_from_memory(runtimeDomain->GetCoreAssemblyImage(), pdbFileData.data(), pdbFileData.size());
    }
}

void EditorScriptEngine::LoadEditorAppAssembly()
{
    auto assemblyPath = Editor::GetProject()->GetLibraryDirectory() / std::format("AppAssembly_{}.dll", assemblyIndex - 1);
    editorDomain->LoadAppAssembly(assemblyPath);
    // debug
    PrintAssemblyTypes(editorDomain->GetAppAssemblyImage());
}
void EditorScriptEngine::LoadRuntimeAppAssembly()
{
    auto assemblyPath = Editor::GetProject()->GetLibraryDirectory() / std::format("AppAssembly_{}.dll", assemblyIndex - 1);
    runtimeDomain->LoadAppAssembly(assemblyPath);

    Path pdbPath = assemblyPath;
    pdbPath.replace_extension(".pdb");
    if (std::filesystem::exists(pdbPath))
    {
        Buffer pdbFileData = FileSystem::ReadBytes(pdbPath);
        mono_debug_open_image_from_memory(runtimeDomain->GetAppAssemblyImage(), pdbFileData.data(), pdbFileData.size());
    }
}

void EditorScriptEngine::PrintAssemblyTypes(MonoImage *image)
{
    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    size_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    for (size_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char *namespaceStr = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char *nameStr = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        Log::CoreDebug("[[APP ASSEMBLY]]: {}.{}", namespaceStr, nameStr);
    }
}

void EditorScriptEngine::WatchScriptFiles(const Path &scriptDir)
{
    scriptFileWatcher = std::make_unique<filewatch::FileWatch<std::filesystem::path>>(
        scriptDir, [this](const std::filesystem::path &file, const filewatch::Event event_type) {
            if (!scriptReloadPending && event_type == filewatch::Event::modified)
            {
                scriptReloadPending = true;
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(100ms);

                Engine::SubmitToMainThread([&]() {
                    ReloadEditorDomain();
                });
            } });
}

void EditorScriptEngine::CompileScripts()
{
    const Path &libraryDir = Editor::GetProject()->GetLibraryDirectory();
    Path appAssemblyPath = libraryDir / std::format("AppAssembly_{}.dll", assemblyIndex++);
    if (!std::filesystem::exists(libraryDir))
    {
        std::filesystem::create_directory(libraryDir);
    }
    Path sourcePath = Editor::GetProject()->GetAssetDirectory() / "scripts" / "*.cs";
    std::string cmd = std::format("csc -target:library -debug:portable -r:ScriptCore.dll -out:{} {}", appAssemblyPath.string(), sourcePath.string());
    Log::CoreInfo("Compiling scripts: {}", cmd);
    std::system(cmd.c_str());

    scriptReloadPending = false;
}

void EditorScriptEngine::OnRuntimeInit()
{
    CreateRuntimeDomain();
    SwitchToRuntime();
    LoadRuntimeCoreAssembly();
    LoadRuntimeAppAssembly();
}
void EditorScriptEngine::OnRuntimeStart()
{
    for (auto &[uuid, entity] : runtimeDomain->GetEntities())
    {
        for (auto &[scriptName, instance] : entity)
        {
            instance->TryInvokeOnCreate();
        }
    }
}

void EditorScriptEngine::OnRuntimeUpdate(float timestep)
{
    for (auto &[uuid, entity] : runtimeDomain->GetEntities())
    {
        for (auto &[scriptName, instance] : entity)
        {
            instance->TryInvokeOnUpdate(timestep);
        }
    }
}

void EditorScriptEngine::OnRuntimeStop()
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