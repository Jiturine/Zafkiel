#pragma once
#include "core/meta/reflection/refl.h"
#include "function/render/texture.h"
#include "core/base/uuid.h"

namespace Zafkiel
{

struct [[refl]] TransformComponent
{
    vec3 position;
    vec3 scale;
};

struct [[refl]] TagComponent
{
    std::string name;
    std::string tag;
};

struct [[refl]] UUIDComponent
{
    UUID id;
};

struct [[refl]] SpriteRendererComponent
{
    vec4 color;
    Ref<Texture2D> texture;
};

struct [[refl]] ScriptComponent
{
    std::vector<std::string> scripts;
};

}