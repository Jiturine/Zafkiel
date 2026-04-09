#include "Core_Base_ImageFormat_refl.h"
#include "Core_Base_UUID_refl.h"
#include "Core_Maths_Maths_refl.h"
#include "Function_RHI_ShaderReflection_refl.h"
#include "Function_Scene_Components_refl.h"
#include "Platform_Filesystem_Filesystem_refl.h"
#include "Resource_Asset_refl.h"
#include "Resource_MaterialAsset_refl.h"

namespace Zafkiel::Reflection 
{
void RegisterEngine() {
    Register_Core_Base_ImageFormat();
    Register_Core_Base_UUID();
    Register_Core_Maths_Maths();
    Register_Function_RHI_ShaderReflection();
    Register_Function_Scene_Components();
    Register_Platform_Filesystem_Filesystem();
    Register_Resource_Asset();
    Register_Resource_MaterialAsset();
}
}