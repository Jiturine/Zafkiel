#include "entity.h"
#include "components.h"
#include "function/engine.h"
#include "function/scene/scene.h"

namespace Zafkiel
{

const Entity Entity::null;

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

void Entity::SetParent(Entity parent)
{
    auto &transform = registry->get<TransformComponent>(handle);
    mat4 prevWorldMatrix = transform.GetWorldMatrix();
    auto prevParent = transform.parent;
    if (prevParent)
    {
        prevParent.RemoveChild(*this);
    }
    transform.parent = parent;
    transform.SetWorldMatrix(prevWorldMatrix);
    if (parent)
        parent.AddChild(*this);
}

void Entity::RemoveChild(Entity child)
{
    auto &children = registry->get<TransformComponent>(handle).children;
    children.erase(std::find(children.begin(), children.end(), child));
}

void Entity::AddChild(Entity child)
{
    auto &children = registry->get<TransformComponent>(handle).children;
    children.push_back(child);
}

UUID Entity::GetUUID() const
{
    return uuid;
}
}