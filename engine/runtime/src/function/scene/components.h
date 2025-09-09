#pragma once
#include "core/meta/reflection/refl.h"
#include "function/render/texture.h"
#include "core/base/uuid.h"
#include "entity.h"

namespace Zafkiel
{

struct [[refl]] TransformComponent
{
    vec3 position;
    quat rotation;
    vec3 scale;
    Entity parent;
    std::vector<Entity> children;

    mutable mat4 worldMatrix = mat4(1.0f);
    mutable bool worldMatrixDirty = true;

    const mat4 &GetWorldMatrix() const
    {
        if (worldMatrixDirty)
        {
            CalculateWorldMatrix();
            worldMatrixDirty = false;
        }
        return worldMatrix;
    }

    void SetPosition(const vec3 &newPosition)
    {
        position = newPosition;
        worldMatrixDirty = true;
    }

    void SetRotation(const quat &newRotation)
    {
        rotation = newRotation;
        worldMatrixDirty = true;
    }

    void SetScale(const vec3 &newScale)
    {
        scale = newScale;
        worldMatrixDirty = true;
    }

  private:
    void CalculateWorldMatrix() const;

    glm::mat4 GetLocalMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), position) *
            glm::mat4_cast(rotation) *
            glm::scale(glm::mat4(1.0f), scale);
    }
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
    AssetHandle texture;
};

struct [[refl]] ScriptComponent
{
    std::vector<std::string> scripts;
};

}