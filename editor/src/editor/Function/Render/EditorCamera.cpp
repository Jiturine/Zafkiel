#include "editor/Function/Render/EditorCamera.h"
#include "Platform/Input/Input.h"

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
    if (Input::IsKeyDown(KeyCode::Q))
    {
        position += moveSpeed * timestep * up;
        RecalculateView();
    }
    if (Input::IsKeyDown(KeyCode::E))
    {
        position -= moveSpeed * timestep * up;
        RecalculateView();
    }
    if (Input::IsKeyDown(KeyCode::A))
    {
        vec3 right = vec3::cross(lookAt, up);
        position -= moveSpeed * timestep * right;
        RecalculateView();
    }
    if (Input::IsKeyDown(KeyCode::D))
    {
        vec3 right = vec3::cross(lookAt, up);
        position += moveSpeed * timestep * right;
        RecalculateView();
    }
    if (Input::IsKeyDown(KeyCode::W))
    {
        position += moveSpeed * timestep * lookAt;
        RecalculateView();
    }
    if (Input::IsKeyDown(KeyCode::S))
    {
        position -= moveSpeed * timestep * lookAt;
        RecalculateView();
    }
    if (Input::IsKeyDown(KeyCode::R))
    {
        lookAt = Maths::Rotate(vec4(lookAt, 1.0f), moveSpeed * 2 * timestep, up);
        RecalculateView();
    }
    if (Input::IsKeyDown(KeyCode::F))
    {
        lookAt = Maths::Rotate(vec4(lookAt, 1.0f), -moveSpeed * 2 * timestep, up);
        RecalculateView();
    }
}
}