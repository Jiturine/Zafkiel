#include "core_base_uuid_refl.h"
#include "core_maths_maths_refl.h"
#include "function_render_shader_family_refl.h"
#include "function_render_shader_reflection_refl.h"
#include "function_scene_components_refl.h"
#include "platform_filesystem_filesystem_refl.h"
#include "resource_asset_refl.h"

namespace Zafkiel::Reflection 
{
void RegisterEngine() {
    Register_core_base_uuid();
    Register_core_maths_maths();
    Register_function_render_shader_family();
    Register_function_render_shader_reflection();
    Register_function_scene_components();
    Register_platform_filesystem_filesystem();
    Register_resource_asset();
}
}