#include "Function/Render/RenderTargetPool.h"
#include "Function/RHI/RHICommandList.h"

namespace Zafkiel 
{
void RenderTargetPool::UpdateTexture(Ref<RHITexture> &oldTexture, uint32 newWidth, uint32 newHeight)
{
    auto textureDesc = oldTexture->GetDesc();
    if (textureDesc.width != newWidth || textureDesc.height != newHeight)
    {
        oldTextures.push_back({0, oldTexture});

        textureDesc.width = newWidth;
        textureDesc.height = newHeight;
        oldTexture = GlobalRHICmdList->CreateTexture(textureDesc);
    }
}

void RenderTargetPool::UpdatePools()
{
    oldTextures.remove_if([&](OldTexture &tex){
        return tex.unusedFrameCount > delayFrames;
    });
    
    for (auto &oldTexture : oldTextures)
    {
        oldTexture.unusedFrameCount++;
    }
}

}

