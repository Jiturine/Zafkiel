#include "maths.h"

namespace Zafkiel
{
namespace Maths
{
mat4 Perspective(float FOV, float aspectRatio, float perspectiveNear, float perspectiveFar)
{
    return glm::perspective(glm::radians(FOV), aspectRatio, perspectiveNear, perspectiveFar);
}

mat4 Ortho(float left, float right, float bottom, float top, float near, float far)
{
    return glm::ortho(left, right, bottom, top, near, far);
}

mat4 LookAt(vec3 eye, vec3 center, vec3 up)
{
    return glm::lookAt(eye, center, up);
}
vec3 EulerRadians(const quat &quaternion)
{
    return glm::eulerAngles(quaternion);
}

vec3 EulerDegrees(const quat &quaternion)
{
    return glm::degrees(glm::eulerAngles(quaternion));
}

quat EulerDrgreesToQuaternion(const vec3 &eulerAngles)
{
    return glm::quat(glm::vec3(glm::radians(eulerAngles.x), glm::radians(eulerAngles.y), glm::radians(eulerAngles.z)));
}

quat EulerRadiansToQuaternion(const vec3 &eulerAngles)
{
    return glm::quat(eulerAngles);
}

vec4 Rotate(const vec4 &vec, float degree, vec3 axis)
{
    return glm::rotate(glm::mat4(1.0f), glm::radians(degree), axis) * vec;
}

bool DecomposeTransform(const mat4 &transform,
    vec3 &out_translation,
    quat &out_rotation,
    vec3 &out_scale)
{
    // 提取平移
    out_translation = vec3(transform[3]);

    // 提取缩放（通过列向量的长度）
    out_scale.x = glm::length(glm::vec3(transform[0]));
    out_scale.y = glm::length(glm::vec3(transform[1]));
    out_scale.z = glm::length(glm::vec3(transform[2]));

    // 去除缩放，得到纯旋转矩阵
    mat3 rotMatrix;
    rotMatrix[0] = vec3(transform[0]) / out_scale.x;
    rotMatrix[1] = vec3(transform[1]) / out_scale.y;
    rotMatrix[2] = vec3(transform[2]) / out_scale.z;

    // 将旋转矩阵转换为四元数
    out_rotation = quat_cast(rotMatrix);

    // 确保四元数归一化（避免数值误差）
    out_rotation = normalize(out_rotation);

    return true;
}

}
}