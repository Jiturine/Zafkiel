#include <spdlog/spdlog.h>
#include "reflection/refl.h"
#include "refl_generate.h"
#include "base/vector3.h"

#include <string>

using namespace Zafkiel;

int main(int argc, char **argv)
{
    ReflectionGenerate::RegisterReflectionInfo();
    vec3 v;
    v.x = 1.0f;
    v.y = 2.0f;
    v.z = 3.0f;
    v.id = 456;
    for (auto &[propType, prop] : Reflection::GetPropertiesOfInstance(v))
    {
        spdlog::info("Property: {} Name: {}", prop.typeinfo->GetName(), propType->GetName());
    }

    return 0;
}