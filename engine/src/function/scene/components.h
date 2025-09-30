#pragma once
#include "core/meta/reflection/refl.h"
#include "core/meta/serializer/custom_serialize.h"
#include "core/base/uuid.h"
#include "core/meta/serializer/deserialize.h"
#include "entity.h"
#include "resource/asset.h"

namespace Zafkiel
{
struct [[refl]] TransformComponent
{
    vec3 position;
    quat rotation;
    vec3 scale = vec3(1.0f);
    Entity parent;
    std::vector<Entity> children;

    TransformComponent() = default;
    TransformComponent(vec3 pos, quat rot = quat(), vec3 scale = vec3(1.0f))
        : position(pos), rotation(rot), scale(scale) {}

    const mat4 &GetWorldMatrix() const;
    mat4 &GetWorldMatrix();

    void SetWorldMatrix(const mat4 &matrix);

    vec3 GetWorldPosition() const;
    quat GetWorldRotation() const;
    vec3 GetWorldScale() const;

    void SetPosition(const vec3 &newPosition);
    void SetRotation(const quat &newRotation);
    void SetScale(const vec3 &newScale);

    void SetParent();
    void AddChild();
    void RemoveChild();

    friend class Entity;
  private:
    mutable mat4 worldMatrix = mat4(1.0f);
    mutable bool worldMatrixDirty = true;

    void MarkDirty();

    void CalculateWorldMatrix() const;

    glm::mat4 GetLocalMatrix() const;
};

template <>
struct Serialization<TransformComponent>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any instance, Any context, YAML::Emitter &out);
    static void Deserialize(Any instance, Any context, const YAML::Node &data);
};

struct [[refl]] TagComponent
{
    std::string name;
    std::string tag;
};

struct [[refl]] SpriteRendererComponent
{
    vec4 color;
    AssetHandle texture;
};

struct [[refl]] MeshComponent
{
    AssetHandle mesh;
};

struct [[refl]] MaterialComponent
{
    AssetHandle material;
};

struct [[refl]] ScriptComponent
{
    UUID entityUUID;
    std::vector<std::string> scripts;
};

template <>
struct Serialization<ScriptComponent>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any instance, Any context, YAML::Emitter &out);
    static void Deserialize(Any instance, Any context, const YAML::Node &data);
};

using ComponentList = std::tuple<TransformComponent, TagComponent, SpriteRendererComponent, MeshComponent, MaterialComponent, ScriptComponent>;

}