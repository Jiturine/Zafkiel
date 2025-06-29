#include "/home/jiturine/Workspace/Zafkiel/engine/runtime/generated/refl_generate.h"

#include "ECS_components.h"
#include "base_vector3.h"

namespace Zafkiel::ReflectionGenerate {

void RegisterReflectionInfo() {
    Register_ECS_components_ReflInfo();
    Register_base_vector3_ReflInfo();
}

}