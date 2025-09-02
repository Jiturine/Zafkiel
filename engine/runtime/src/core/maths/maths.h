#pragma once
#include "core/meta/reflection/refl.h"
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

struct [[refl]] vec3 : public glm::vec3
{
    using glm::vec3::vec3;
    vec3(glm::vec3 raw) : glm::vec3(raw) {}
    static vec3 cross(vec3 param1, vec3 param2)
    {
        return glm::cross(param1, param2);
    }
};

struct [[refl]] vec4 : public glm::vec4
{
    using glm::vec4::vec4;
    vec4(glm::vec4 raw) : glm::vec4(raw) {}
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
};

struct [[refl]] testStruct
{
    float oneProp;
    float anotherProp;
    float evenMoreProp;
};

namespace Maths
{
mat4 Perspective(float FOV, float aspectRatio, float perspectiveNear, float perspectiveFar);

mat4 Ortho(float left, float right, float bottom, float top, float near, float far);

mat4 LookAt(vec3 eye, vec3 center, vec3 up);
}

}