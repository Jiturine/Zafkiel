#pragma once
#include <imgui.h>
#include "ImGuizmo.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include "function/window/window.h"

namespace Zafkiel
{
class EditorGUI
{
  public:
    EditorGUI() = default;
    template <typename... Args>
    EditorGUI Text(std::format_string<Args...> format, Args &&...args)
    {
        if (sameLine) ImGui::SameLine();
        ImGui::Text("%s", std::format(format, std::forward<Args>(args)...).c_str());
        sameLine = true;
        return *this;
    }
    EditorGUI CheckBox(const std::string &label, bool &value)
    {
        if (sameLine) ImGui::SameLine();
        ImGui::Checkbox(label.c_str(), &value);
        sameLine = true;
        return *this;
    }
    EditorGUI Button(const std::string &label, std::function<void()> onClick)
    {
        if (sameLine) ImGui::SameLine();
        if (ImGui::Button(label.c_str())) onClick();
        sameLine = true;
        return *this;
    }
    EditorGUI InvisibleButton(const std::string &label, vec2 size)
    {
        if (sameLine) ImGui::SameLine();
        ImGui::InvisibleButton(label.c_str(), ImVec2(size.x, size.y));
        sameLine = true;
        return *this;
    }
    EditorGUI Image(uint32_t id, vec2 size, vec2 uv0 = vec2(0.0f, 0.0f), vec2 uv1 = vec2(1.0f, 1.0f))
    {
        if (sameLine) ImGui::SameLine();
        ImGui::Image(id, ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y));
        sameLine = true;
        return *this;
    }
    EditorGUI SameLine()
    {
        ImGui::SameLine();
        return *this;
    }
    EditorGUI InputText(const std::string &label, std::string &text)
    {
        if (sameLine) ImGui::SameLine();
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, text.c_str());
        if (ImGui::InputText(std::format("{}##{}", label, reinterpret_cast<uintptr_t>(&buffer)).c_str(), buffer, sizeof(buffer)))
        {
            text = std::string(buffer);
        }
        sameLine = true;
        return *this;
    }
    EditorGUI DragInt(const std::string &label, int &data, std::function<void(void)> onValueChanged = nullptr, float speed = 0.5f)
    {
        if (sameLine) ImGui::SameLine();
        if (ImGui::DragInt(std::format("{}##{}", label, reinterpret_cast<uintptr_t>(&data)).c_str(), &data, speed))
        {
            if (onValueChanged) onValueChanged();
        }
        sameLine = true;
        return *this;
    }
    EditorGUI DragFloat(const std::string &label, float &data, std::function<void(void)> onValueChanged = nullptr, float speed = 0.5f)
    {
        if (sameLine) ImGui::SameLine();
        if (ImGui::DragFloat(std::format("{}##{}", label, reinterpret_cast<uintptr_t>(&data)).c_str(), &data, speed))
        {
            if (onValueChanged) onValueChanged();
        }
        sameLine = true;
        return *this;
    }
    EditorGUI DragVec2(const std::string &label, vec2 &data, std::function<void(void)> onValueChanged = nullptr, float speed = 0.5f)
    {
        if (sameLine) ImGui::SameLine();
        if (ImGui::DragFloat2(std::format("{}##{}", label, reinterpret_cast<uintptr_t>(&data)).c_str(), (float *)&data, speed))
        {
            if (onValueChanged) onValueChanged();
        }
        sameLine = true;
        return *this;
    }
    EditorGUI DragVec3(const std::string &label, vec3 &data, std::function<void(void)> onValueChanged = nullptr, float speed = 0.5f)
    {
        if (sameLine) ImGui::SameLine();
        if (ImGui::DragFloat3(std::format("{}##{}", label, reinterpret_cast<uintptr_t>(&data)).c_str(), (float *)&data, speed))
        {
            if (onValueChanged) onValueChanged();
        }
        sameLine = true;
        return *this;
    }
    EditorGUI DragVec4(const std::string &label, vec4 &data, std::function<void(void)> onValueChanged = nullptr, float speed = 0.5f)
    {
        if (sameLine) ImGui::SameLine();
        if (ImGui::DragFloat4(std::format("{}##{}", label, reinterpret_cast<uintptr_t>(&data)).c_str(), (float *)&data, speed))
        {
            if (onValueChanged) onValueChanged();
        }
        sameLine = true;
        return *this;
    }
    EditorGUI MenuItem(const std::string &label, std::function<void(void)> onClick)
    {
        if (ImGui::MenuItem(label.c_str()))
        {
            onClick();
        }
        return *this;
    }

    static void StartFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }
    static void EndFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
    }

  private:
    bool sameLine = false;
};

class GUIWindow
{
  public:
    GUIWindow(const std::string &name) : name(name)
    {
        ImGui::Begin(name.c_str());
        hovered = ImGui::IsWindowHovered();
    }
    ~GUIWindow()
    {
        ImGui::End();
    }
    vec2 GetWindowPosition()
    {
        ImVec2 pos = ImGui::GetWindowPos();
        return vec2(pos.x, pos.y);
    }
    vec2 GetContentPosition()
    {
        ImVec2 region = ImGui::GetWindowContentRegionMin();
        ImVec2 windowPos = ImGui::GetWindowPos();
        return vec2(windowPos.x + region.x, windowPos.y + region.y);
    }
    vec2 GetWindowSize()
    {
        ImVec2 size = ImGui::GetWindowSize();
        return vec2(size.x, size.y);
    }
    vec2 GetContentSize()
    {
        ImVec2 contentMin = ImGui::GetCursorScreenPos();
        ImVec2 relativeContentMax = ImGui::GetWindowContentRegionMax();
        vec2 contentMax = GetWindowPosition() + vec2(relativeContentMax.x, relativeContentMax.y);
        ImVec2 size = ImVec2(contentMax.x - contentMin.x, contentMax.y - contentMin.y);
        return vec2(size.x, size.y);
    }
    void Popup(std::function<void(void)> onPopup)
    {
        if (ImGui::BeginPopupContextWindow(std::format("{} popup", name).c_str(), ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            onPopup();
            ImGui::EndPopup();
        }
    }
    void OnClickEmpty(std::function<void(void)> onClickEmpty)
    {
        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            onClickEmpty();
        }
    }
    bool hovered;
  private:
    std::string name;
};
}

class GUITreeNode
{
  public:
    GUITreeNode(uint32_t id, ImGuiTreeNodeFlags flags, const std::string &name)
        : nodeID(reinterpret_cast<void *>((uintptr_t)id))
    {
        opened = ImGui::TreeNodeEx(nodeID, flags, "%s", name.c_str());
        leftClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        rightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
    }
    GUITreeNode(const void *id, ImGuiTreeNodeFlags flags, const std::string &name)
        : nodeID(id)
    {
        opened = ImGui::TreeNodeEx(nodeID, flags, "%s", name.c_str());
        leftClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        rightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
    }
    void Expand(std::function<void(void)> expandFunc)
    {
        if (opened)
        {
            expandFunc();
        }
    }
    void Popup(std::function<void(void)> popupFunc)
    {
        if (ImGui::BeginPopupContextItem(std::format("{}", nodeID).c_str(), ImGuiPopupFlags_MouseButtonRight))
        {
            popupFunc();
            ImGui::EndPopup();
        }
    }
    ~GUITreeNode()
    {
        if (opened)
        {
            ImGui::TreePop();
        }
    }
    const void *nodeID;
    bool opened;
    bool leftClicked;
    bool rightClicked;
};

class GUIDockSpace
{
  public:
    GUIDockSpace(const std::string &name)
    {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(name.c_str(), nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID(name.c_str());
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    ~GUIDockSpace()
    {
        ImGui::End();
    }
};