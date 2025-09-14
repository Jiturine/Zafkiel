#include "components.h"
#include "entity.h"
#include "function/scene/scene.h"
#include "function/engine.h"

namespace Zafkiel
{
const mat4 &TransformComponent::GetWorldMatrix() const
{
    if (worldMatrixDirty)
    {
        CalculateWorldMatrix();
        worldMatrixDirty = false;
    }
    return worldMatrix;
}

void TransformComponent::SetPosition(const vec3 &newPosition)
{
    position = newPosition;
    worldMatrixDirty = true;
}

void TransformComponent::SetRotation(const quat &newRotation)
{
    rotation = newRotation;
    worldMatrixDirty = true;
}

void TransformComponent::SetScale(const vec3 &newScale)
{
    scale = newScale;
    worldMatrixDirty = true;
}
void TransformComponent::CalculateWorldMatrix() const
{
    if (parent)
    {
        auto &parentTransform = parent.GetComponent<TransformComponent>();
        worldMatrix = parentTransform.GetWorldMatrix() * GetLocalMatrix();
    }
    else
    {
        worldMatrix = GetLocalMatrix();
    }
}
glm::mat4 TransformComponent::GetLocalMatrix() const
{
    return glm::translate(glm::mat4(1.0f), position) *
        glm::mat4_cast(rotation) *
        glm::scale(glm::mat4(1.0f), scale);
}

void Serialization<TransformComponent>::Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
{
    auto &transform = instance.As<TransformComponent>();
    out << YAML::BeginMap;
    out << YAML::Key << "position" << YAML::Value;
    SerializeAny(transform.position, GetType<vec3>(), out);
    out << YAML::Key << "rotation" << YAML::Value;
    SerializeAny(transform.rotation, GetType<quat>(), out);
    out << YAML::Key << "scale" << YAML::Value;
    SerializeAny(transform.scale, GetType<vec3>(), out);
    if (transform.parent)
    {
        out << YAML::Key << "parent" << YAML::Value;
        auto uuid = transform.parent.GetUUID();
        SerializeAny(uuid, GetType<UUID>(), out);
    }
    if (!transform.children.empty())
    {
        out << YAML::Key << "children" << YAML::Value << YAML::BeginSeq;
        for (auto child : transform.children)
        {
            auto uuid = transform.parent.GetUUID();
            SerializeAny(uuid, GetType<UUID>(), out);
        }
        out << YAML::EndSeq;
    }
    out << YAML::EndMap;
}

void Serialization<TransformComponent>::Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data)
{
    auto &transform = instance.As<TransformComponent>();
    Any posInstance = transform.position;
    DeserializeAny(posInstance, GetType<vec3>(), data["position"]);
    Any rotInstance = transform.rotation;
    DeserializeAny(rotInstance, GetType<quat>(), data["rotation"]);
    Any scaleInstance = transform.scale;
    DeserializeAny(scaleInstance, GetType<vec3>(), data["scale"]);
    if (data["parent"])
    {
        transform.parent = Engine::GetCurrentScene()->GetWorld().GetEntityByUUID(data["parent"].as<uint64_t>());
    }
    if (data["children"])
    {
        for (auto child : data["children"])
        {
            transform.children.push_back(Engine::GetCurrentScene()->GetWorld().GetEntityByUUID(child.as<uint64_t>()));
        }
    }
}

}