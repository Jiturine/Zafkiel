#include "entity.h"
#include "components.h"

namespace Zafkiel
{

bool Entity::HasParent() const
{
    return registry->all_of<TransformComponent>(handle) && registry->get<TransformComponent>(handle).parent;
}

Entity Entity::GetParent() const
{
    return registry->get<TransformComponent>(handle).parent;
}

bool Entity::HasChildren() const
{
    return registry->all_of<TransformComponent>(handle) && !registry->get<TransformComponent>(handle).children.empty();
}

std::vector<Entity> Entity::GetChildren() const
{
    return registry->get<TransformComponent>(handle).children;
}
UUID Entity::GetUUID() const
{
    return uuid;
}
}