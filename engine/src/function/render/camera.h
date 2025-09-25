#pragma once

namespace Zafkiel
{
class Camera
{
  public:
    enum class ProjectionType
    {
        Perspective = 0,
        Orthographic = 1
    };

  public:
    Camera() = default;
    virtual ~Camera() = default;

    void SetOrthographic(float size, float nearClip, float farClip);
    void SetPerspective(float verticalFOV, float nearClip, float farClip);
    void SetViewportSize(unsigned int width, unsigned int height);

    ProjectionType GetProjectionType() const { return projectionType; }
    void SetProjectionType(ProjectionType type)
    {
        projectionType = type;
        RecalculateProjection();
    }

    float GetPerspectiveVerticalFOV() const { return perspectiveFOV; }
    void SetPerspectiveVerticalFOV(float fov)
    {
        perspectiveFOV = fov;
        RecalculateProjection();
    }
    float GetPerspectiveNearClip() const { return perspectiveNear; }
    float GetPerspectiveFarClip() const { return perspectiveFar; }

    void SetPerspectiveNearClip(float nearClip)
    {
        perspectiveNear = nearClip;
        RecalculateProjection();
    }
    void SetPerspectiveFarClip(float farClip)
    {
        perspectiveFar = farClip;
        RecalculateProjection();
    }

    float GetOrthographicSize() const { return orthographicSize; }
    void SetOrthographicSize(float size)
    {
        orthographicSize = size;
        RecalculateProjection();
    }

    float GetOrthographicNearClip() const { return orthographicNear; }
    float GetOrthographicFarClip() const { return orthographicFar; }

    void SetOrthographicNearClip(float nearClip)
    {
        orthographicNear = nearClip;
        RecalculateProjection();
    }
    void SetOrthographicFarClip(float farClip)
    {
        orthographicFar = farClip;
        RecalculateProjection();
    }
    const mat4 &GetProjectionMatrix() const { return projectionMatrix; }

  private:
    void RecalculateProjection();
    mat4 projectionMatrix;

    float orthographicSize = 10.0f;
    float orthographicNear = -1.0f, orthographicFar = 1.0f;
    float aspectRatio = 1.0f;

    float perspectiveFOV = 45.0f;
    float perspectiveNear = 0.01f, perspectiveFar = 1000.0f;

    ProjectionType projectionType = ProjectionType::Orthographic;
};
}