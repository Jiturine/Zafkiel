#include "Core/Base/Memory.h"

struct LiveEntry
{
    bool alive = false;
    uint32 generation = 0;
};

static std::unordered_map<void *, LiveEntry> liveReferences;
static std::mutex liveReferenceMutex;

namespace Zafkiel
{

namespace RefUtils
{
void AddToLiveReferences(void *instance)
{
    std::scoped_lock<std::mutex> lock(liveReferenceMutex);
    if (!instance)
    {
        Log::Error("instance is nullptr");
        return;
    }
    auto &entry = liveReferences[instance];
    entry.alive = true;
    entry.generation++;
}

void RemoveFromLiveReferences(void *instance)
{
    std::scoped_lock<std::mutex> lock(liveReferenceMutex);
    if (!instance)
    {
        Log::Error("instance is nullptr");
        return;
    }
    auto it = liveReferences.find(instance);
    if (it != liveReferences.end())
        it->second.alive = false;
}

uint32 GetGeneration(void *instance)
{
    if (!instance)
        return 0;
    auto it = liveReferences.find(instance);
    if (it != liveReferences.end())
        return it->second.generation;
    return 0;
}

bool Check(void *instance, uint32 generation)
{
    if (!instance)
        return false;
    auto it = liveReferences.find(instance);
    return it != liveReferences.end() && it->second.alive && it->second.generation == generation;
}

}

}