#include "editor_refl.h"
#include "engine_extensions_resource_editor_asset_manager_refl.h"
#include "project_project_refl.h"

namespace Zafkiel::Reflection 
{
void RegisterEditor() {
    Register_editor();
    Register_engine_extensions_resource_editor_asset_manager();
    Register_project_project();
}
}