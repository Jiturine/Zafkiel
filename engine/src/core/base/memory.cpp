#include "memory.h"

static std::unordered_set<void *> liveReferences;
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
    }
    liveReferences.insert(instance);
}

void RemoveFromLiveReferences(void *instance)
{
    std::scoped_lock<std::mutex> lock(liveReferenceMutex);
    if (!instance)
    {
        Log::Error("instance is nullptr");
    }
    liveReferences.erase(instance);
}

bool IsLive(void *instance)
{

    if (!instance)
    {
        Log::Error("instance is nullptr");
    }
    return liveReferences.contains(instance);
}

}

}