#pragma once
#include "core/meta/reflection/refl.h"
#include "core/meta/serializer/custom_serialize.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Zafkiel
{
struct [[refl]] vec2 : public glm::vec2
{
    using glm::vec2::vec2;
    vec2(glm::vec2 raw) : glm::vec2(raw) {}
};

template <>
struct Serialization<vec2>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
    {
        const vec2 obj = instance.As<vec2>();
        out << YAML::Flow << YAML::BeginSeq << obj.x << obj.y << YAML::EndSeq;
    }
    static void Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data)
    {
        vec2 &obj = instance.As<vec2>();
        obj.x = data[0].as<float>();
        obj.y = data[1].as<float>();
    }
};

struct [[refl]] vec3 : public glm::vec3
{
    using glm::vec3::vec3;
    vec3(glm::vec3 raw) : glm::vec3(raw) {}
    static vec3 cross(vec3 param1, vec3 param2)
    {
        return glm::cross(param1, param2);
    }
};

template <>
struct Serialization<vec3>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
    {
        const vec3 obj = instance.As<vec3>();
        out << YAML::Flow << YAML::BeginSeq << obj.x << obj.y << obj.z << YAML::EndSeq;
    }
    static void Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data)
    {
        vec3 &obj = instance.As<vec3>();
        obj.x = data[0].as<float>();
        obj.y = data[1].as<float>();
        obj.z = data[2].as<float>();
    }
};

struct [[refl]] vec4 : public glm::vec4
{
    using glm::vec4::vec4;
    vec4(glm::vec4 raw) : glm::vec4(raw) {}
};

template <>
struct Serialization<vec4>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
    {
        const vec4 obj = instance.As<vec4>();
        out << YAML::Flow << YAML::BeginSeq << obj.x << obj.y << obj.z << obj.w << YAML::EndSeq;
    }
    static void Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data)
    {
        vec4 &obj = instance.As<vec4>();
        obj.x = data[0].as<float>();
        obj.y = data[1].as<float>();
        obj.z = data[2].as<float>();
        obj.w = data[3].as<float>();
    }
};

struct [[refl]] quat : public glm::quat
{
    using glm::quat::quat;
    quat(glm::quat raw) : glm::quat(raw) {}
};

template <>
struct Serialization<quat>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
    {
        const quat obj = instance.As<quat>();
        out << YAML::Flow << YAML::BeginSeq << obj.x << obj.y << obj.z << obj.w << YAML::EndSeq;
    }
    static void Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data)
    {
        quat &obj = instance.As<quat>();
        obj.x = data[0].as<float>();
        obj.y = data[1].as<float>();
        obj.z = data[2].as<float>();
        obj.w = data[3].as<float>();
    }
};

struct [[refl]] mat3 : public glm::mat3
{
    using glm::mat3::mat3;
    mat3(glm::mat3 raw) : glm::mat3(raw) {}
};

struct [[refl]] mat4 : public glm::mat4
{
    using glm::mat4::mat4;
    mat4(glm::mat4 raw) : glm::mat4(raw) {}
    const float *value() const { return glm::value_ptr(*static_cast<const glm::mat4 *>(this)); }
    float *value() { return glm::value_ptr(*static_cast<glm::mat4 *>(this)); }
};

namespace Maths
{
mat4 Perspective(float FOV, float aspectRatio, float perspectiveNear, float perspectiveFar);

mat4 Ortho(float left, float right, float bottom, float top, float near, float far);

mat4 LookAt(vec3 eye, vec3 center, vec3 up);

vec3 EulerRadians(const quat &quaternion);

vec3 EulerDegrees(const quat &quaternion);

quat EulerDrgreesToQuaternion(const vec3 &eulerAngles);

quat EulerRadiansToQuaternion(const vec3 &eulerAngles);

vec4 Rotate(const vec4 &vec, float degree, vec3 axis);

bool DecomposeTransform(const mat4 &transform,
    vec3 &out_translation,
    quat &out_rotation,
    vec3 &out_scale);

}

}