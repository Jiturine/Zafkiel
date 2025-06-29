#pragma once
#include "reflection/refl.h"
#include "base/vector3.h"

namespace Zafkiel::ReflectionGenerate {

inline void Register_base_vector3_ReflInfo() {
    

    ::Zafkiel::Reflection::Register<::Zafkiel::vec3>("vec3")
    .AddProperty(&::Zafkiel::vec3::x, "x")
    .AddProperty(&::Zafkiel::vec3::y, "y")
    .AddProperty(&::Zafkiel::vec3::z, "z")
    ;
        
}

}