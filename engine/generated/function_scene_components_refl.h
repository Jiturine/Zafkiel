#pragma once
#include "core/meta/reflection/refl.h"
#include "function/scene/components.h"

namespace Zafkiel::Reflection 
{
inline void Register_function_scene_components() 
{
    ::Zafkiel::Reflection::Register<::Zafkiel::LightType>("LightType")
    .Add(::Zafkiel::LightType::Directional, "Directional")
    .Add(::Zafkiel::LightType::Point, "Point")
    .Add(::Zafkiel::LightType::Spot, "Spot")
    ;
 

    ::Zafkiel::Reflection::Register<::Zafkiel::TransformComponent>("TransformComponent")
    .AddProperty(&::Zafkiel::TransformComponent::position, "position")
    .AddProperty(&::Zafkiel::TransformComponent::rotation, "rotation")
    .AddProperty(&::Zafkiel::TransformComponent::scale, "scale")
    .AddProperty(&::Zafkiel::TransformComponent::parent, "parent")
    .AddProperty(&::Zafkiel::TransformComponent::children, "children")
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::TagComponent>("TagComponent")
    .AddProperty(&::Zafkiel::TagComponent::name, "name")
    .AddProperty(&::Zafkiel::TagComponent::tag, "tag")
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::SpriteRendererComponent>("SpriteRendererComponent")
    .AddProperty(&::Zafkiel::SpriteRendererComponent::color, "color")
    .AddProperty(&::Zafkiel::SpriteRendererComponent::texture, "texture")
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::MeshComponent>("MeshComponent")
    .AddProperty(&::Zafkiel::MeshComponent::mesh, "mesh")
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::MaterialComponent>("MaterialComponent")
    .AddProperty(&::Zafkiel::MaterialComponent::material, "material")
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::LightComponent>("LightComponent")
    .AddProperty(&::Zafkiel::LightComponent::type, "type")
    .AddProperty(&::Zafkiel::LightComponent::color, "color")
    .AddProperty(&::Zafkiel::LightComponent::intensity, "intensity")
    .AddProperty(&::Zafkiel::LightComponent::direction, "direction")
    .AddProperty(&::Zafkiel::LightComponent::radius, "radius")
    .AddProperty(&::Zafkiel::LightComponent::spotAngle, "spotAngle")
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::ScriptComponent>("ScriptComponent")
    .AddProperty(&::Zafkiel::ScriptComponent::entityUUID, "entityUUID")
    .AddProperty(&::Zafkiel::ScriptComponent::scripts, "scripts")
    ;
}
}