#include "editor_camera.h"
#include "function/input/input.h"

namespace Zafkiel
{
EditorCamera::EditorCamera()
{
    RecalculateView();
}

void EditorCamera::RecalculateView()
{
    viewMatrix = Maths::LookAt(position, position + lookAt, up);
}

void EditorCamera::Update(float timestep)
{
    if (Input::IsKeyPressed(Scancode::Q))
    {
        position += moveSpeed * timestep * up;
        RecalculateView();
    }
    if (Input::IsKeyPressed(Scancode::E))
    {
        position -= moveSpeed * timestep * up;
        RecalculateView();
    }
    if (Input::IsKeyPressed(Scancode::A))
    {
        vec3 right = vec3::cross(lookAt, up);
        position -= moveSpeed * timestep * right;
        RecalculateView();
    }
    if (Input::IsKeyPressed(Scancode::D))
    {
        vec3 right = vec3::cross(lookAt, up);
        position += moveSpeed * timestep * right;
        RecalculateView();
    }
    if (Input::IsKeyPressed(Scancode::W))
    {
        position += moveSpeed * timestep * lookAt;
        RecalculateView();
    }
    if (Input::IsKeyPressed(Scancode::S))
    {
        position -= moveSpeed * timestep * lookAt;
        RecalculateView();
    }
}
}