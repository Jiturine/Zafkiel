#pragma once
#include "Function/RHI/RHIResources.h"
#include "Core/Base/Utils.h"

namespace Zafkiel
{

class RenderTargetPool
{
  public:
    void UpdateTexture(Ref<RHITexture> &oldTexture, uint32 newWidth, uint32 newHeight);

    void UpdatePools();
  
  private:
    struct OldTexture
    {
        uint32 unusedFrameCount;
        Ref<RHITexture> handle;
    };

    uint32 delayFrames = 2; 

    uint32 currentFrame = 0;

    std::list<OldTexture> oldTextures;
};

}