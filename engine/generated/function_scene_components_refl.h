#pragma once
#include "core/meta/reflection/refl.h"
#include "function/scene/components.h"

namespace Zafkiel::Reflection 
{
inline void Register_function_scene_components() 
{
    

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
    ::Zafkiel::Reflection::Register<::Zafkiel::ScriptComponent>("ScriptComponent")
    .AddProperty(&::Zafkiel::ScriptComponent::scripts, "scripts")
    ;
}
}