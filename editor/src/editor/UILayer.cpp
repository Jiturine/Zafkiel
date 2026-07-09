#include "editor/UILayer.h"

#include "Function/UI/Border.h"
#include "Function/UI/SubWindow.h"
#include "Function/UI/Splitter.h"
#include "Function/UI/Text.h"
#include "Function/UI/Docking/DockSpace.h"
#include "Function/UI/Docking/DockTabStack.h"
#include "editor/EditorLayer.h"
#include "EngineReflGenerate.h"
#include "EditorReflGenerate.h"
#include "editor/EditorGUI/EditorGUI.h"

#include "Platform/Input/Input.h"

#include "Core/Application/Application.h"
#include "Core/Async/TaskGraph.h"

#include "Function/RHI/Backends/Vulkan/VulkanRenderPass.h"
#include "Function/Render/Renderer.h"

#include "Function/UI/Rect.h"
#include "Function/UI/UISystem.h"

#include "Function/Scene/Components.h"
#include "editor/Project/ProjectManager.h"

#include "Core/Meta/Serializer/BinarySerializer.h"

#include "editor/Function/Scene/EditorSceneManager.h"

namespace Zafkiel
{

void UILayer::OnAttach()
{
    Reflection::RegisterEditor();
    Reflection::RegisterEngine();

    TaskGraph::Init();

    SDL_Init(SDL_INIT_VIDEO);

    PlatformApplication::Init();

    PlatformWindowSpecification spec
    {
        .graphicsAPI = GraphicsAPI::OpenGL,
        .title = "Zafkiel Editor",
        .width = 1920,
        .height = 1080
    };
    auto platformWindow = CreateRef<PlatformWindow>(spec);

    Renderer::Init(GraphicsAPI::OpenGL);

    Fence initFence;
    TaskGraph::Instance().EnqueueTask(NamedThreadType::RenderThread, [self = Ref(this), platformWindow, &initFence]() mutable {
        self->viewport = GlobalRHI->CreateViewport(platformWindow.get());
        self->uiPass = CreateScope<UIPass>();
        UISystem::Init();
        initFence.Signal();
    });
    initFence.Wait();

    auto mainWindow = CreateWidget<Window>(platformWindow.get());
    mainWindow->SetViewport(viewport.get());

    PlatformApplication::Instance().SetEventHandler(&UISystem::Instance());

    PlatformApplication::Instance().AddPlatformWindow(platformWindow);
    UISystem::Instance().AddWindow(mainWindow);

#if 0
    auto blueQuad = CreateWidget<Rect>(vec4(0.0f, 0.2f, 0.3f, 1.0f));
    auto yellowQuad = CreateWidget<Rect>(vec4(0.8f, 0.8f, 0.1f, 1.0f), vec2(200, 200));
    auto redQuad = CreateWidget<Rect>(vec4(0.9f, 0.2f, 0.1f, 1.0f));
    auto demoText = CreateWidget<Text>(L"Hello World! This is a long text...\nHello GUI!", UISystem::Instance().GetDefaultFont(), 40.0f, vec3(0.0f, 0.0f, 0.0f), 300.0f);

    auto panel = CreateRef<Panel>(LayoutRule::Horizontal, Panel::Padding{}, Panel::Space{10.0f, 10.0f});
    panel->AddChild(Panel::Slot()[yellowQuad].SetSizeRule(SizeRule::Auto).SetVerticalAlignmentRule(VerticalAlignmentRule::Bottom));
    panel->AddChild(Panel::Slot()[redQuad].SetSizeRule(SizeRule::Grow).SetVerticalAlignmentRule(VerticalAlignmentRule::Fill));
    panel->AddChild(Panel::Slot()[demoText].SetSizeRule(SizeRule::Auto));

    auto border = CreateRef<Border>(Border::Padding{10.0f, 10.0f, 10.0f, 10.0f});
    border->SetBackground(SingleWidgetSlot()[blueQuad].SetSizeRule(SizeRule::Grow));
    border->SetForeground(SingleWidgetSlot()[panel].SetSizeRule(SizeRule::Grow));
    mainWindow->SetSlot(SingleWidgetSlot()[border].SetSizeRule(SizeRule::Grow));
#endif 

#if 0
    auto blueQuad = CreateWidget<Rect>(vec4(0.0f, 0.2f, 0.3f, 1.0f));
    auto yellowQuad = CreateWidget<Rect>(vec4(0.8f, 0.8f, 0.1f, 1.0f), vec2(200, 200));
    auto redQuad = CreateWidget<Rect>(vec4(0.9f, 0.2f, 0.1f, 1.0f));
    auto demoText = CreateWidget<Text>(L"Hello World! This is a long text...\nHello GUI!", UISystem::Instance().GetDefaultFont(), 40.0f, vec3(0.0f, 0.0f, 0.0f), 300.0f);

    auto splitter = CreateRef<Splitter>(LayoutRule::Horizontal);
    splitter->AddChild(Splitter::Slot()[yellowQuad].SetSizeFactor(0.3f));
    splitter->AddChild(Splitter::Slot()[redQuad].SetSizeFactor(0.4f));
    splitter->AddChild(Splitter::Slot()[demoText].SetSizeFactor(0.3f));

    auto border = CreateRef<Border>();
    border->SetBackground(Border::BorderBackgroundSlot()[blueQuad]);
    border->SetContent(SingleWidgetSlot()[splitter].SetSizeRule(SizeRule::Grow));
    mainWindow->SetContent(SingleWidgetSlot()[border].SetSizeRule(SizeRule::Grow));
#endif 

#if 0
    auto subWindow = CreateWidget<SubWindow>(vec2(100, 100), vec2(200, 200));
    auto blueQuad = CreateWidget<Rect>(vec4(0.0f, 0.2f, 0.3f, 1.0f));
    subWindow->SetContent(SingleWidgetSlot()[blueQuad].SetSizeRule(SizeRule::Grow));
    mainWindow->SetSlot(SingleWidgetSlot()[subWindow].SetSizeRule(SizeRule::Auto));
#endif 

#if 1
    auto rect = CreateWidget<Rect>(vec4(0.0f, 0.0f, 1.0f, 1.0f));
    auto dockTab = CreateWidget<DockTab>(L"Untitled Tab");
    dockTab->SetTabContent(rect);

    auto dockTabStack = CreateWidget<DockTabStack>();
    dockTabStack->OpenTab(dockTab);

    auto floatingDockSpace = CreateWidget<DockSpace>();
    floatingDockSpace->AddChild(dockTabStack);

    auto subWindow = CreateWidget<DockSubWindow>(vec2(200, 200), vec2(500, 400));
    subWindow->SetContent(SingleWidgetSlot().SetSizeRule(SizeRule::Grow)[floatingDockSpace]);

    floatingDockSpace->SetParentSubWindow(subWindow.get());
    
    // ---
    auto rect2 = CreateWidget<Rect>(vec4(0.0f, 0.3f, 0.5f, 1.0f));
    auto dockTab2= CreateWidget<DockTab>(L"Untitled Tab 2");
    dockTab2->SetTabContent(rect2);

    auto dockTabStack2 = CreateWidget<DockTabStack>();
    dockTabStack2->OpenTab(dockTab2);

    auto floatingDockSpace2 = CreateWidget<DockSpace>();
    floatingDockSpace2->AddChild(dockTabStack2);

    auto subWindow2 = CreateWidget<DockSubWindow>(vec2(200, 700), vec2(300, 300));
    subWindow2->SetContent(SingleWidgetSlot().SetSizeRule(SizeRule::Grow)[floatingDockSpace2]);

    floatingDockSpace2->SetParentSubWindow(subWindow2.get());
    // ---

    auto dockSpace = CreateWidget<DockSpace>();
    dockSpace->AddSubWindow(subWindow);

    dockSpace->AddSubWindow(subWindow2);

    mainWindow->SetContent(SingleWidgetSlot()[dockSpace].SetSizeRule(SizeRule::Grow));
    
#endif
    windows.push_back(mainWindow);

    // ProjectManager::Init();
    // const std::string editorConfigStr = FileSystem::ReadText("editor_config.yaml");
    // EditorConfig config = Deserialize<EditorConfig>(editorConfigStr);

    // const std::string projectConfigStr = FileSystem::ReadText(config.startProjectPath);
    // ProjectConfig projConfig = Deserialize<ProjectConfig>(projectConfigStr);
    // ProjectManager::CreateProject(projConfig);

    // EditorAssetManager::Init();
    // EditorAssetManager::Instance().SetAssetDirectory(ProjectManager::GetCurrentProject()->GetAssetDirectory());
    // EditorAssetManager::Instance().RegisterAssets();

    // EditorSceneManager::Init();
    // EditorSceneManager::Instance().OpenScene(projConfig.startScene);

    // Application::Instance().FlushRenderingCommands();
}

void UILayer::OnDetach()
{
    Fence destroyFence;

    TaskGraph::Instance().EnqueueTask(NamedThreadType::RenderThread, [&]() {
        GlobalRHI->WaitIdle();

        windows.clear();

        viewport = nullptr;
        uiPass = nullptr;

        GlobalRHI->WaitIdle();

        destroyFence.Signal();
    });

    destroyFence.Wait();

    UISystem::Destroy();

    EditorSceneManager::Destroy();
    EditorAssetManager::Destroy();
    ProjectManager::Destroy();
    Renderer::Destroy();

    TaskGraph::Destroy();
}

void UILayer::OnUpdate(float timestep)
{
    if (PlatformApplication::Instance().ShouldQuit())
    {
        Application::Instance().Exit();
        return;
    }

    PlatformApplication::Instance().PollEvents();

    UISystem::Instance().QueryCursor();

    Fence fence;
    TaskGraph::Instance().EnqueueTask(NamedThreadType::RenderThread, [this, &fence]() {
        Renderer::Instance().GetRenderTargetPool().UpdatePools();
        
        uiPass->Render(windows);

        GlobalRHICmdList->FinalizeContext();

        GlobalRHICmdList->Submit();

        fence.Signal();
    });

    fence.Wait();
}

}
