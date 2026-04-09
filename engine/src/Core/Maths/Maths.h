#pragma once
#include "Core/Meta/Reflection/Refl.h"
#include "Core/Meta/Serializer/CustomSerialize.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Function/RHI/GraphicsAPI.h"

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
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const vec2 obj = instance.As<vec2>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        vec2 &obj = instance.As<vec2>();
        obj.x = data[0].As<float>();
        obj.y = data[1].As<float>();
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
    static float dot(vec3 param1, vec3 param2)
    {
        return glm::dot(static_cast<const glm::vec3&>(param1), static_cast<const glm::vec3&>(param2));
    }
};

template <>
struct Serialization<vec3>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const vec3 obj = instance.As<vec3>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).Value(obj.z).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        vec3 &obj = instance.As<vec3>();
        obj.x = data[0].As<float>();
        obj.y = data[1].As<float>();
        obj.z = data[2].As<float>();
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
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const vec4 obj = instance.As<vec4>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).Value(obj.z).Value(obj.w).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        vec4 &obj = instance.As<vec4>();
        obj.x = data[0].As<float>();
        obj.y = data[1].As<float>();
        obj.z = data[2].As<float>();
        obj.w = data[3].As<float>();
    }
};

struct [[refl]] uvec2 : public glm::uvec2
{
    using glm::uvec2::uvec2;
    uvec2(glm::uvec2 raw) : glm::uvec2(raw) {}
};

template <>
struct Serialization<uvec2>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const uvec2 obj = instance.As<uvec2>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        uvec2 &obj = instance.As<uvec2>();
        obj.x = data[0].As<uint32>();
        obj.y = data[1].As<uint32>();
    }
};

struct [[refl]] uvec3 : public glm::uvec3
{
    using glm::uvec3::uvec3;
    uvec3(glm::uvec3 raw) : glm::uvec3(raw) {}
};

template <>
struct Serialization<uvec3>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const uvec3 obj = instance.As<uvec3>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).Value(obj.z).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        uvec3 &obj = instance.As<uvec3>();
        obj.x = data[0].As<uint32>();
        obj.y = data[1].As<uint32>();
        obj.z = data[2].As<uint32>();
    }
};

struct [[refl]] uvec4 : public glm::uvec4
{
    using glm::uvec4::uvec4;
    uvec4(glm::uvec4 raw) : glm::uvec4(raw) {}
};

template <>
struct Serialization<uvec4>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const uvec4 obj = instance.As<uvec4>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).Value(obj.z).Value(obj.w).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        uvec4 &obj = instance.As<uvec4>();
        obj.x = data[0].As<uint32>();
        obj.y = data[1].As<uint32>();
        obj.z = data[2].As<uint32>();
        obj.w = data[3].As<uint32>();
    }
};

struct [[refl]] ivec2 : public glm::ivec2
{
    using glm::ivec2::ivec2;
    ivec2(glm::ivec2 raw) : glm::ivec2(raw) {}
};

template <>
struct Serialization<ivec2>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const ivec2 obj = instance.As<ivec2>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        ivec2 &obj = instance.As<ivec2>();
        obj.x = data[0].As<int>();
        obj.y = data[1].As<int>();
    }
};

struct [[refl]] ivec3 : public glm::ivec3
{
    using glm::ivec3::ivec3;
    ivec3(glm::ivec3 raw) : glm::ivec3(raw) {}
};

template <>
struct Serialization<ivec3>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const ivec3 obj = instance.As<ivec3>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).Value(obj.z).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        ivec3 &obj = instance.As<ivec3>();
        obj.x = data[0].As<int>();
        obj.y = data[1].As<int>();
        obj.z = data[2].As<int>();
    }
};

struct [[refl]] ivec4 : public glm::ivec4
{
    using glm::ivec4::ivec4;
    ivec4(glm::ivec4 raw) : glm::ivec4(raw) {}
};

template <>
struct Serialization<ivec4>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const ivec4 obj = instance.As<ivec4>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).Value(obj.z).Value(obj.w).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        ivec4 &obj = instance.As<ivec4>();
        obj.x = data[0].As<int>();
        obj.y = data[1].As<int>();
        obj.z = data[2].As<int>();
        obj.w = data[3].As<int>();
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
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const quat obj = instance.As<quat>();
        out.BeginBatchSeq().Value(obj.x).Value(obj.y).Value(obj.z).Value(obj.w).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        quat &obj = instance.As<quat>();
        obj.x = data[0].As<float>();
        obj.y = data[1].As<float>();
        obj.z = data[2].As<float>();
        obj.w = data[3].As<float>();
    } 
};

struct [[refl]] mat3 : public glm::mat3
{
    using glm::mat3::mat3;
    mat3(glm::mat3 raw) : glm::mat3(raw) {}
};

template <>
struct Serialization<mat3>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const mat3 obj = instance.As<mat3>();
        out.BeginSeq().Value((vec3)obj[0]).Value((vec3)obj[1]).Value((vec3)obj[2]).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        mat3 &obj = instance.As<mat3>();
        obj[0] = data[0].As<vec3>();
        obj[1] = data[1].As<vec3>();
        obj[2] = data[2].As<vec3>();
    } 
};

struct [[refl]] mat4 : public glm::mat4
{
    using glm::mat4::mat4;
    mat4(glm::mat4 raw) : glm::mat4(raw) {}
    const float *value() const { return glm::value_ptr(*static_cast<const glm::mat4 *>(this)); }
    float *value() { return glm::value_ptr(*static_cast<glm::mat4 *>(this)); }
};

template <>
struct Serialization<mat4>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const AnyRef instance, AnyRef context, ISerializer &out)
    {
        const mat4 obj = instance.As<mat4>();
        out.BeginSeq().Value((vec4)obj[0]).Value((vec4)obj[1]).Value((vec4)obj[2]).Value((vec4)obj[3]).EndSeq();
    }
    static void Deserialize(AnyRef instance, AnyRef context, IDeserializer &data)
    {
        mat4 &obj = instance.As<mat4>();
        obj[0] = data[0].As<vec4>();
        obj[1] = data[1].As<vec4>();
        obj[2] = data[2].As<vec4>();
        obj[3] = data[3].As<vec4>();
    } 
};

namespace Maths
{
void SetAPI(GraphicsAPI api);

GraphicsAPI GetAPI();

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