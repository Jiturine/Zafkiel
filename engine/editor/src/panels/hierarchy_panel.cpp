#include "hierarchy_panel.h"
#include "editorGUI/editorGUI.h"
#include "function/scene/components.h"

namespace Zafkiel
{

void HierarchyPanel::Render()
{
    GUIWindow hierarchyPanel("Hierarchy");
    for (auto entity : scene->GetWorld().AllEntities())
    {
    }
}
}