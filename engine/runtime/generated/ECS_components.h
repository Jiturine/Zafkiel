#pragma once
#include "reflection/refl.h"
#include "ECS/components.h"

namespace Zafkiel::ReflectionGenerate {

inline void Register_ECS_components_ReflInfo() {
    

    ::Zafkiel::Reflection::Register<::Zafkiel::TransformComponent>("TransformComponent")
    .AddProperty(&::Zafkiel::TransformComponent::position, "position")
    .AddProperty(&::Zafkiel::TransformComponent::scale, "scale")
    ;
        
}

}