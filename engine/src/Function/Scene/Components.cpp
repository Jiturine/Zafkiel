#include "Function/Scene/Components.h"
#include "Function/Scene/Entity.h"
#include "Function/Scene/Scene.h"
#include "Function/Script/ScriptEngine.h"

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

void TransformComponent::SetLocalMatrix(const mat4 &matrix)
{
    Maths::DecomposeTransform(matrix, position, rotation, scale);
    MarkDirty();
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

void Serialization<TransformComponent>::Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
{
    auto &transform = instance.As<TransformComponent>();
    out.BeginMap();
    out.Key("position").Value(transform.position);
    out.Key("rotation").Value(transform.rotation);
    out.Key("scale").Value(transform.scale);

    if (transform.parent)
    {
        out.Key("parent").Value(transform.parent.GetUUID());
    }
    if (!transform.children.empty())
    {
        out.Key("children").BeginSeq();
        for (auto child : transform.children)
        {
            out.Value(child.GetUUID());
        }
        out.EndSeq();
    }
    out.EndMap();
}

void Serialization<TransformComponent>::Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
{
    World &world = context.As<World>();
    auto &transform = instance.As<TransformComponent>();
    transform.position = data["position"].As<vec3>();
    transform.rotation = data["rotation"].As<quat>();
    transform.scale = data["scale"].As<vec3>();
    if (data["parent"])
    {
        transform.parent = world.GetEntityByUUID(data["parent"].As<uint64_t>());
    }
    if (data["children"])
    {
        auto &childrenData = data["children"];

        for (size_t i = 0; i < childrenData.Size(); i++)
        {
            UUID childUUID = childrenData[i].As<UUID>();
            transform.children.push_back(world.GetEntityByUUID(childUUID));
        }
    }
}

void Serialization<LightComponent>::Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
{
    auto &lightComponent = instance.As<LightComponent>();
    out.BeginMap();
    out.Key("Type").Value(lightComponent.type);
    out.Key("Color").Value(lightComponent.color);
    out.Key("Intensity").Value(lightComponent.intensity);
    if (lightComponent.type == LightType::Directional)
    {
        out.Key("Direction").Value(lightComponent.direction);
    }
    else if (lightComponent.type == LightType::Point)
    {
        out.Key("Radius").Value(lightComponent.radius);
    }
    else if (lightComponent.type == LightType::Spot)
    {
        out.Key("SpotAngle").Value(lightComponent.spotAngle);
    }
    out.EndMap();
}

void Serialization<LightComponent>::Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
{
    auto &lightComponent = instance.As<LightComponent>();
    lightComponent.type = data["Type"].As<LightType>();
    lightComponent.color = data["Color"].As<vec3>();
    lightComponent.intensity = data["intensity"].As<float>();
    if (lightComponent.type == LightType::Directional)
    {
        lightComponent.direction = data["Direction"].As<vec3>();
    }
    else if (lightComponent.type == LightType::Point)
    {
        lightComponent.radius = data["Radius"].As<float>();
    }
    else if (lightComponent.type == LightType::Spot)
    {
        lightComponent.spotAngle = data["SpotAngle"].As<float>();
    }
}

void Serialization<ScriptComponent>::Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
{
    auto &scriptComponent = instance.As<ScriptComponent>();
    auto &scripts = scriptComponent.scripts;
    ScriptInstanceMap &scriptInstances = context.As<ScriptInstanceMap>();
    out.BeginMap();
    for (auto &scriptName : scripts)
    {
        out.Key(scriptName);
        out.BeginMap();
        auto scriptInstance = scriptInstances[scriptName];
        if (!scriptInstance)
        {
            out.Key("Error").Value("Script instance not found");
            out.EndMap();
            continue;
        }
        for (const auto &[fieldName, field] : scriptInstance->GetScriptClass()->GetFields())
        {
            out.Key(fieldName);
            switch (field.type)
            {
                using enum ScriptFieldType;
            case Float: out.Value(scriptInstance->GetFieldValue<float>(fieldName)); break;
            case Double: out.Value(scriptInstance->GetFieldValue<double>(fieldName)); break;
            case Char: out.Value(scriptInstance->GetFieldValue<char>(fieldName)); break;
            case Int16: out.Value(scriptInstance->GetFieldValue<int16_t>(fieldName)); break;
            case Int32: out.Value(scriptInstance->GetFieldValue<int32_t>(fieldName)); break;
            case Int64: out.Value(scriptInstance->GetFieldValue<int64_t>(fieldName)); break;
            case UInt8: out.Value(scriptInstance->GetFieldValue<uint8>(fieldName)); break;
            case UInt16: out.Value(scriptInstance->GetFieldValue<uint16_t>(fieldName)); break;
            case UInt32: out.Value(scriptInstance->GetFieldValue<uint32>(fieldName)); break;
            case UInt64: out.Value(scriptInstance->GetFieldValue<uint64_t>(fieldName)); break;
            case Bool: out.Value(scriptInstance->GetFieldValue<bool>(fieldName)); break;
            case Vector2: out.Value(scriptInstance->GetFieldValue<vec2>(fieldName)); break;
            case Vector3: out.Value(scriptInstance->GetFieldValue<vec3>(fieldName)); break;
            case Vector4: out.Value(scriptInstance->GetFieldValue<vec4>(fieldName)); break;
            case Entity: out.Value(scriptInstance->GetFieldValue<uint64_t>(fieldName)); break;
            default:
                break;
            }
        }
        out.EndMap();
    }
    out.EndMap();
}

void Serialization<ScriptComponent>::Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
{
    auto &scriptComponent = instance.As<ScriptComponent>();
    auto &entity = context.As<Entity>();
    scriptComponent.entityUUID = entity.GetUUID();

    for (auto &item : data.MapItems())
    {
        std::string scriptName = item.first.As<std::string>();
        auto &scriptInstanceNode = data[scriptName];
        scriptComponent.scripts.push_back(scriptName);
        auto scriptInstance = ScriptEngine::AddScriptInstance(entity.GetUUID(), scriptName);
        for (auto &[fieldName, field] : scriptInstance->GetScriptClass()->GetFields())
        {
            if (!scriptInstanceNode[fieldName]) continue;
            auto &fieldNode = scriptInstanceNode[fieldName];
            switch (field.type)
            {
                using enum ScriptFieldType;
            case Float: scriptInstance->SetFieldValue<float>(fieldName, fieldNode.As<float>()); break;
            case Double: scriptInstance->SetFieldValue<double>(fieldName, fieldNode.As<double>()); break;
            case Char: scriptInstance->SetFieldValue<char>(fieldName, fieldNode.As<char>()); break;
            case Int16: scriptInstance->SetFieldValue<int16_t>(fieldName, fieldNode.As<int16_t>()); break;
            case Int32: scriptInstance->SetFieldValue<int32_t>(fieldName, fieldNode.As<int32_t>()); break;
            case Int64: scriptInstance->SetFieldValue<int64_t>(fieldName, fieldNode.As<int64_t>()); break;
            case UInt8: scriptInstance->SetFieldValue<uint8>(fieldName, fieldNode.As<uint8>()); break;
            case UInt16: scriptInstance->SetFieldValue<uint16_t>(fieldName, fieldNode.As<uint16_t>()); break;
            case UInt32: scriptInstance->SetFieldValue<uint32>(fieldName, fieldNode.As<uint32>()); break;
            case UInt64: scriptInstance->SetFieldValue<uint64_t>(fieldName, fieldNode.As<uint64_t>()); break;
            case Bool: scriptInstance->SetFieldValue<bool>(fieldName, fieldNode.As<bool>()); break;
            case Vector2: scriptInstance->SetFieldValue<vec2>(fieldName, fieldNode.As<vec2>()); break;
            case Vector3: scriptInstance->SetFieldValue<vec3>(fieldName, fieldNode.As<vec3>()); break;
            case Vector4: scriptInstance->SetFieldValue<vec4>(fieldName, fieldNode.As<vec4>()); break;
            case Entity: scriptInstance->SetFieldValue<uint64_t>(fieldName, fieldNode.As<uint64_t>()); break;
            default:
                break;
            }
        }
    }
}
}