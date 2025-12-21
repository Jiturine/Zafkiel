#include "core_base_uuid_refl.h"
#include "core_maths_maths_refl.h"
#include "function_render_fragment_module_refl.h"
#include "function_render_global_render_resource_refl.h"
#include "function_render_graphics_context_refl.h"
#include "function_render_graphics_pipeline_refl.h"
#include "function_render_graphics_shader_refl.h"
#include "function_render_material_refl.h"
#include "function_render_mesh_refl.h"
#include "function_render_model_refl.h"
#include "function_render_object_render_resource_refl.h"
#include "function_render_render_command_refl.h"
#include "function_render_render_pass_resource_refl.h"
#include "function_render_render_resource_refl.h"
#include "function_render_render_resource_template_refl.h"
#include "function_render_renderer_refl.h"
#include "function_render_shader_refl.h"
#include "function_render_shader_family_refl.h"
#include "function_render_shader_module_refl.h"
#include "function_render_shader_reflection_refl.h"
#include "function_render_uniform_buffer_refl.h"
#include "function_render_vertex_buffer_refl.h"
#include "function_render_vertex_module_refl.h"
#include "function_scene_components_refl.h"
#include "platform_filesystem_filesystem_refl.h"
#include "resource_asset_refl.h"
#include "resource_asset_manager_refl.h"

namespace Zafkiel::Reflection 
{
void RegisterEngine() {
    Register_core_base_uuid();
    Register_core_maths_maths();
    Register_function_render_fragment_module();
    Register_function_render_global_render_resource();
    Register_function_render_graphics_context();
    Register_function_render_graphics_pipeline();
    Register_function_render_graphics_shader();
    Register_function_render_material();
    Register_function_render_mesh();
    Register_function_render_model();
    Register_function_render_object_render_resource();
    Register_function_render_render_command();
    Register_function_render_render_pass_resource();
    Register_function_render_render_resource();
    Register_function_render_render_resource_template();
    Register_function_render_renderer();
    Register_function_render_shader();
    Register_function_render_shader_family();
    Register_function_render_shader_module();
    Register_function_render_shader_reflection();
    Register_function_render_uniform_buffer();
    Register_function_render_vertex_buffer();
    Register_function_render_vertex_module();
    Register_function_scene_components();
    Register_platform_filesystem_filesystem();
    Register_resource_asset();
    Register_resource_asset_manager();
}
}