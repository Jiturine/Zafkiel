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

}
}