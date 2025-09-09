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

}
}