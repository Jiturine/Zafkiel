#include "function/render/camera.h"
#include "function/render/renderer.h"
#include <iterator>

namespace Zafkiel
{
void Camera::SetOrthographic(float size, float nearClip, float farClip)
{
    projectionType = ProjectionType::Orthographic;
    orthographicSize = size;
    orthographicNear = nearClip;
    orthographicFar = farClip;
    RecalculateProjection();
}
void Camera::SetPerspective(float verticalFOV, float nearClip, float farClip)
{
    projectionType = ProjectionType::Perspective;
    perspectiveFOV = verticalFOV;
    perspectiveNear = nearClip;
    perspectiveFar = farClip;
    RecalculateProjection();
}
void Camera::SetViewportSize(uint32_t width, uint32_t height)
{
    if (width <= 0 || height <= 0)
    {
        Log::Error("Viewport size must be larger than 0!");
    }
    aspectRatio = (float)width / (float)height;
    RecalculateProjection();
}
void Camera::RecalculateProjection()
{
    if (projectionType == ProjectionType::Perspective)
    {
        projectionMatrix = Maths::Perspective(perspectiveFOV, aspectRatio, perspectiveNear, perspectiveFar);
    }
    else if (projectionType == ProjectionType::Orthographic)
    {
        float left = -aspectRatio * orthographicSize * 0.5f;
        float right = aspectRatio * orthographicSize * 0.5f;
        float bottom = -orthographicSize * 0.5f;
        float top = orthographicSize * 0.5f;
        projectionMatrix = Maths::Ortho(left, right, bottom, top, orthographicNear, orthographicFar);
    }
}
}
