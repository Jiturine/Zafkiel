#include "components.h"
#include "entity.h"
#include "function/scene/scene.h"
#include "function/engine.h"
#include "function/script/script_engine.h"

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

mat4 &TransformComponent::GetWorldMatrix()
{
    if (worldMatrixDirty)
    {
        CalculateWorldMatrix();
        worldMatrixDirty = false;
    }
    return worldMatrix;
}

void TransformComponent::SetWorldMatrix(const mat4 &matrix)
{
    mat4 localMatrix;
    if (parent)
    {
        const mat4 &parentWorldMatrix = parent.GetComponent<TransformComponent>().worldMatrix;
        localMatrix = glm::inverse(parentWorldMatrix) * matrix;
    }
    else
    {
        localMatrix = matrix;
    }
    Maths::DecomposeTransform(localMatrix, position, rotation, scale);
    MarkDirty();
}

vec3 TransformComponent::GetWorldPosition() const
{
    vec3 worldPosition;
    quat worldRotation;
    vec3 worldScale;
    Maths::DecomposeTransform(GetWorldMatrix(), worldPosition, worldRotation, worldScale);
    return worldPosition;
}
quat TransformComponent::GetWorldRotation() const
{
    vec3 worldPosition;
    quat worldRotation;
    vec3 worldScale;
    Maths::DecomposeTransform(GetWorldMatrix(), worldPosition, worldRotation, worldScale);
    return worldRotation;
}
vec3 TransformComponent::GetWorldScale() const
{
    vec3 worldPosition;
    quat worldRotation;
    vec3 worldScale;
    Maths::DecomposeTransform(GetWorldMatrix(), worldPosition, worldRotation, worldScale);
    return worldScale;
}

void TransformComponent::SetPosition(const vec3 &newPosition)
{
    position = newPosition;
    MarkDirty();
}

void TransformComponent::SetRotation(const quat &newRotation)
{
    rotation = newRotation;
    MarkDirty();
}

void TransformComponent::SetScale(const vec3 &newScale)
{
    scale = newScale;
    MarkDirty();
}

void TransformComponent::MarkDirty()
{
    worldMatrixDirty = true;
    for (auto child : children)
    {
        child.GetComponent<TransformComponent>().MarkDirty();
    }
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

void Serialization<TransformComponent>::Serialize(const Any instance, Any context, YAML::Emitter &out)
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
            auto uuid = child.GetUUID();
            SerializeAny(uuid, GetType<UUID>(), out);
        }
        out << YAML::EndSeq;
    }
    out << YAML::EndMap;
}

void Serialization<TransformComponent>::Deserialize(Any instance, Any context, const YAML::Node &data)
{
    World &world = context.As<World>();
    auto &transform = instance.As<TransformComponent>();
    Any posInstance = transform.position;
    DeserializeAny(posInstance, GetType<vec3>(), data["position"]);
    Any rotInstance = transform.rotation;
    DeserializeAny(rotInstance, GetType<quat>(), data["rotation"]);
    Any scaleInstance = transform.scale;
    DeserializeAny(scaleInstance, GetType<vec3>(), data["scale"]);
    if (data["parent"])
    {
        transform.parent = world.GetEntityByUUID(data["parent"].as<uint64_t>());
    }
    if (data["children"])
    {
        for (auto child : data["children"])
        {
            transform.children.push_back(world.GetEntityByUUID(child.as<uint64_t>()));
        }
    }
}

void Serialization<ScriptComponent>::Serialize(const Any instance, Any context, YAML::Emitter &out)
{
    auto &scriptComponent = instance.As<ScriptComponent>();
    auto &scripts = scriptComponent.scripts;
    ScriptInstanceMap &scriptInstances = context.As<ScriptInstanceMap>();
    out << YAML::BeginMap;
    for (auto &scriptName : scripts)
    {
        out << YAML::Key << scriptName << YAML::Value;
        out << YAML::BeginMap;
        auto scriptInstance = scriptInstances[scriptName];
        if (!scriptInstance)
        {
            out << YAML::Key << "Error" << YAML::Value << "Script instance not found";
            out << YAML::EndMap;
            continue;
        }
        for (const auto &[fieldName, field] : scriptInstance->GetScriptClass()->GetFields())
        {
            out << YAML::Key << fieldName << YAML::Value;
            switch (field.type)
            {
                using enum ScriptFieldType;
            case Float: out << scriptInstance->GetFieldValue<float>(fieldName); break;
            case Double: out << scriptInstance->GetFieldValue<double>(fieldName); break;
            case Char: out << scriptInstance->GetFieldValue<char>(fieldName); break;
            case Int16: out << scriptInstance->GetFieldValue<int16_t>(fieldName); break;
            case Int32: out << scriptInstance->GetFieldValue<int32_t>(fieldName); break;
            case Int64: out << scriptInstance->GetFieldValue<int64_t>(fieldName); break;
            case UInt8: out << scriptInstance->GetFieldValue<uint8_t>(fieldName); break;
            case UInt16: out << scriptInstance->GetFieldValue<uint16_t>(fieldName); break;
            case UInt32: out << scriptInstance->GetFieldValue<uint32_t>(fieldName); break;
            case UInt64: out << scriptInstance->GetFieldValue<uint64_t>(fieldName); break;
            case Bool: out << scriptInstance->GetFieldValue<bool>(fieldName); break;
            case Vector2: {
                vec2 instance = scriptInstance->GetFieldValue<vec2>(fieldName);
                SerializeAny(instance, GetType<vec2>(), out);
                break;
            }
            case Vector3: {
                vec3 instance = scriptInstance->GetFieldValue<vec3>(fieldName);
                SerializeAny(instance, GetType<vec3>(), out);
                break;
            }
            case Vector4: {
                vec4 instance = scriptInstance->GetFieldValue<vec4>(fieldName);
                SerializeAny(instance, GetType<vec4>(), out);
                break;
            }
            case Entity: out << scriptInstance->GetFieldValue<uint64_t>(fieldName); break;
            default:
                break;
            }
        }
        out << YAML::EndMap;
    }
    out << YAML::EndMap;
}

void Serialization<ScriptComponent>::Deserialize(Any instance, Any context, const YAML::Node &data)
{
    auto &scriptComponent = instance.As<ScriptComponent>();
    auto [scriptEngine, entity] = context.As<std::pair<Ref<ScriptEngine>, Entity>>();
    scriptComponent.entityUUID = entity.GetUUID();
    for (auto it : data)
    {
        const auto &scriptName = it.first.as<std::string>();
        const auto &scriptInstanceNode = it.second;
        scriptComponent.scripts.push_back(scriptName);
        auto scriptInstance = scriptEngine->AddScriptInstance(entity.GetUUID(), scriptName);
        for (const auto &[fieldName, field] : scriptInstance->GetScriptClass()->GetFields())
        {
            if (!scriptInstanceNode[fieldName].IsDefined()) continue;
            switch (field.type)
            {
                using enum ScriptFieldType;
            case Float: scriptInstance->SetFieldValue<float>(fieldName, scriptInstanceNode[fieldName].as<float>()); break;
            case Double: scriptInstance->SetFieldValue<double>(fieldName, scriptInstanceNode[fieldName].as<double>()); break;
            case Char: scriptInstance->SetFieldValue<char>(fieldName, scriptInstanceNode[fieldName].as<char>()); break;
            case Int16: scriptInstance->SetFieldValue<int16_t>(fieldName, scriptInstanceNode[fieldName].as<int16_t>()); break;
            case Int32: scriptInstance->SetFieldValue<int32_t>(fieldName, scriptInstanceNode[fieldName].as<int32_t>()); break;
            case Int64: scriptInstance->SetFieldValue<int64_t>(fieldName, scriptInstanceNode[fieldName].as<int64_t>()); break;
            case UInt8: scriptInstance->SetFieldValue<uint8_t>(fieldName, scriptInstanceNode[fieldName].as<uint8_t>()); break;
            case UInt16: scriptInstance->SetFieldValue<uint16_t>(fieldName, scriptInstanceNode[fieldName].as<uint16_t>()); break;
            case UInt32: scriptInstance->SetFieldValue<uint32_t>(fieldName, scriptInstanceNode[fieldName].as<uint32_t>()); break;
            case UInt64: scriptInstance->SetFieldValue<uint64_t>(fieldName, scriptInstanceNode[fieldName].as<uint64_t>()); break;
            case Bool: scriptInstance->SetFieldValue<bool>(fieldName, scriptInstanceNode[fieldName].as<bool>()); break;
            case Vector2: {
                vec2 instance;
                Any a = instance;
                DeserializeAny(a, GetType<vec2>(), scriptInstanceNode[fieldName]);
                scriptInstance->SetFieldValue<vec2>(fieldName, a.As<vec2>());
                break;
            }
            case Vector3: {
                vec3 instance;
                Any a = instance;
                DeserializeAny(a, GetType<vec3>(), scriptInstanceNode[fieldName]);
                scriptInstance->SetFieldValue<vec3>(fieldName, a.As<vec3>());
                break;
            }
            case Vector4: {
                vec4 instance;
                Any a = instance;
                DeserializeAny(a, GetType<vec4>(), scriptInstanceNode[fieldName]);
                scriptInstance->SetFieldValue<vec4>(fieldName, a.As<vec4>());
                break;
            }
            case Entity: scriptInstance->SetFieldValue<uint64_t>(fieldName, scriptInstanceNode[fieldName].as<uint64_t>()); break;
            default:
                break;
            }
        }
    }
}
}