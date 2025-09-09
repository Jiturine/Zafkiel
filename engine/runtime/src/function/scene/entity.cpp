#include "entity.h"
#include "components.h"

namespace Zafkiel
{

bool Entity::HasParent() const
{
    return registry->get<TransformComponent>(handle).parent;
}

Entity Entity::GetParent() const
{
    return registry->get<TransformComponent>(handle).parent;
}
}