#include "editor_refl.h"
#include "project_project_refl.h"
#include "resource_editor_asset_manager_refl.h"

namespace Zafkiel::Reflection 
{
void RegisterEditor() {
    Register_editor();
    Register_project_project();
    Register_resource_editor_asset_manager();
}
}