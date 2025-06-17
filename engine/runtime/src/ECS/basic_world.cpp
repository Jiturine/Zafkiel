#include "basic_world.h"

void BasicWorld::Destroy(EntityID entity)
{
    if (auto it = entities.find(entity); it != entities.end())
    {
        for (auto [id, component] : it->second)
        {
            auto &componentInfo = componentMap[id];
            componentInfo.pool.Destroy(component);
            componentInfo.sparseSet.Remove(entity);
        }
        entities.erase(it);
    }
}