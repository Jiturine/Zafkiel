#include "core_base_uuid_refl.h"
#include "core_maths_maths_refl.h"
#include "function_scene_components_refl.h"
#include "function_scene_scene_refl.h"
#include "function_scene_world_refl.h"
#include "platform_filesystem_filesystem_refl.h"
#include "resource_asset_refl.h"
#include "resource_asset_manager_refl.h"
#include "resource_runtime_asset_manager_refl.h"

namespace Zafkiel::Reflection 
{
void RegisterEngine() {
    Register_core_base_uuid();
    Register_core_maths_maths();
    Register_function_scene_components();
    Register_function_scene_scene();
    Register_function_scene_world();
    Register_platform_filesystem_filesystem();
    Register_resource_asset();
    Register_resource_asset_manager();
    Register_resource_runtime_asset_manager();
}
}