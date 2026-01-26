#pragma once
#include "core/meta/reflection/refl.h"
#include "core/meta/serializer/custom_serialize.h"
#include "core/base/uuid.h"
#include "function/scene/entity.h"
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

    void SetLocalMatrix(const mat4 &matrix);
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
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out);
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data);
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

enum class [[refl]] LightType
{
    Directional,
    Point,
    Spot
};

struct [[refl]] LightComponent
{
    LightType type = LightType::Directional;

    vec3 color = vec3(1.0f);
    float intensity = 1.0f;

    // 方向光
    vec3 direction = vec3(0.0f, -1.0f, 0.0f);

    // 点光/聚光灯
    float radius = 10.0f; // 衰减范围

    // 聚光灯特有
    float spotAngle = glm::radians(30.0f);
};

template <>
struct Serialization<LightComponent>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out);
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data);
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
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out);
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data);
};

using ComponentList = std::tuple<TransformComponent, TagComponent, SpriteRendererComponent, MeshComponent, MaterialComponent, LightComponent, ScriptComponent>;

}