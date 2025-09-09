#include "components.h"
#include "entity.h"

namespace Zafkiel
{
void TransformComponent::CalculateWorldMatrix() const
{
    if (parent)
    {
        auto &parentTransform = parent.GetComponent<TransformComponent>();
        worldMatrix = parentTransform.GetWorldMatrix() * GetLocalMatrix();
    }
    else
    {
        worldMatrix = GetLocalMatrix();
    }
}

}