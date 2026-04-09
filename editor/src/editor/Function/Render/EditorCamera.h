#pragma once
#include "Function/Render/Camera.h"

namespace Zafkiel
{
class EditorCamera final : public Camera
{
  public:
    EditorCamera();
    const mat4 &GetViewMatrix() const { return viewMatrix; }
    void SetPosition(const vec3 &position)
    {
        this->position = position;
        RecalculateView();
    }
    vec3 GetPosition() const { return position; }
    void SetLookAtDir(const vec3 &lookAt)
    {
        this->lookAt = lookAt;
        RecalculateView();
    }

    void Update(float timestep);

  private:
    void RecalculateView();

    vec3 position = vec3(0.0f, 0.0f, 0.0f);
    vec3 lookAt = vec3(0.0f, 0.0f, 1.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    mat4 viewMatrix;
    float moveSpeed = 10.0f;
};
}