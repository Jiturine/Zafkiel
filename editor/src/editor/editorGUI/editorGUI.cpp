#include "editor/editorGUI/editorGUI.h"
#include "function/render/backends/vulkan/vulkan_context.h"
#include "function/render/backends/vulkan/vulkan_render_pass.h"
#include "function/render/renderer.h"
#include <glad/glad.h>

namespace Zafkiel 
{

void EditorGUI::BeginFrame()
{
    GraphicsAPI API = Renderer::GetGraphicsContext()->GetAPI();
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
    if (Renderer::GetGraphicsContext()->GetAPI() == GraphicsAPI::OpenGL)
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
    else if (Renderer::GetGraphicsContext()->GetAPI() == GraphicsAPI::Vulkan)
    {
        auto &vulkanContext = Renderer::GetGraphicsContext().As<VulkanContext>();
        ImDrawData* drawData = ImGui::GetDrawData();
        vulkanContext.RenderToScreen([&]() {
            ImGui_ImplVulkan_RenderDrawData(drawData, Renderer::Instance().GetGraphicsContext().As<VulkanContext>().GetCommandBuffer());
        });
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
}

}
