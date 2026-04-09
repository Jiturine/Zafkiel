#include "editor/Panels/ContentBrowserPanel.h"
#include "editor/EditorGUI/EditorGUI.h"
#include "editor/Resource/EditorAssetManager.h"

namespace Zafkiel
{
void ContentBrowserPanel::Render()
{
    GUIWindow contentBrowserPanel("Content Browser");

    const Path &assetDirectory = EditorAssetManager::Instance().GetAssetDirectory();

    EditorGUI().Text("{}", currentDirectory.RelativeTo(assetDirectory).string()).SameLine();

    if (currentDirectory != assetDirectory)
    {
        EditorGUI().Button("..", [&]() {
            currentDirectory = currentDirectory.parent_path();
        });
    }
    ImGui::Separator();

    for (auto &directoryEntry : std::filesystem::directory_iterator(currentDirectory))
    {
        ImGui::PushID(directoryEntry.path().string().c_str());
        const Path &path = directoryEntry.path();
        std::string filename = path.filename().string();

        const char *label = filename.c_str();
        EditorGUI().Text("{}", label);
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (directoryEntry.is_directory())
            {
                currentDirectory /= directoryEntry.path().filename();
            }
        }
        ImGui::PopID();
    }
}

void ContentBrowserPanel::SetCurrentDirectory(const Path &dir)
{
    currentDirectory = dir;
}

}