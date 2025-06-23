#include "basic_world.h"

void BasicWorld::Destroy(EntityID entity)
{
    if (auto it = entities.find(entity); it != entities.end())
    {
        for (auto cid : it->second)
        {
            componentMap[cid]->Remove(entity);
        }
        entities.erase(it);
    }
}