#include "editor/EditorGUI/EditorGUI.h"
#include "Function/RHI/Backends/Vulkan/VulkanRenderPass.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanRHI.h"
#include "Function/Render/Renderer.h"
#include <glad/glad.h>

namespace Zafkiel 
{
#if 0
void EditorGUI::BeginFrame()
{
    GraphicsAPI API = GlobalRHI->GetAPI();
    if (API == GraphicsAPI::OpenGL)
        ImGui_ImplOpenGL3_NewFrame();
    else if (API == GraphicsAPI::Vulkan)
        ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}
void EditorGUI::EndFrame()
{
    ImGui::Render();
    if (GlobalRHI->GetAPI() == GraphicsAPI::OpenGL)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    else if (GlobalRHI->GetAPI() == GraphicsAPI::Vulkan)
    {
        auto vulkanContext = static_cast<VulkanGraphicsContext *>(GlobalRHICmdList->GetGraphicsContext());
        ImDrawData* drawData = ImGui::GetDrawData();

        auto &swapchain = GlobalRHI.As<VulkanRHI>().GetDevice().GetSwapchain();

        uint32 index = swapchain.AcquireNextImageIndex();

        auto swapchainTexture = swapchain.GetTexture(index);

        RHIRenderPassInfo renderPassInfo
        {
            .colorAttachments
            {
                {
                    .texture = swapchainTexture,
                    .clearValue = { .vec4Value = vec4(0.1, 0.1, 0.1, 1) },
                    .initialLayout = ImageLayout::Undefined,
                    .finalLayout = ImageLayout::PresentSrc,
                }
            }
        };
        GlobalRHICmdList->BeginRenderPass(renderPassInfo);

        ImGui_ImplVulkan_RenderDrawData(drawData, *vulkanContext->GetCommandBuffer()->GetHandle());

        GlobalRHICmdList->EndRenderPass();

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
}
#endif
}
