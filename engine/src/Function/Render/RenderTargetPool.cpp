#include "Function/Render/RenderTargetPool.h"
#include "Function/RHI/RHICommandList.h"

namespace Zafkiel 
{
void RenderTargetPool::UpdateTexture(Ref<RHITexture> &oldTexture, uint32 newWidth, uint32 newHeight)
{
    RHITextureDesc textureDesc = oldTexture->GetDesc();
    if (textureDesc.width != newWidth || textureDesc.height != newHeight)
    {
        oldTextures.push_back({0, oldTexture});

        textureDesc.width = newWidth;
        textureDesc.height = newHeight;
        oldTexture = GlobalRHICmdList->CreateTexture(textureDesc);

        if (record.contains(oldTexture.get()))
            Log::Warn("exists!!!");
        record.insert(oldTexture.get());
    }
}

void RenderTargetPool::UpdatePools()
{
    oldTextures.remove_if([&](OldTexture &tex){

        if (tex.unusedFrameCount > delayFrames)
        {
            record.erase(tex.handle.get());
            return true;
        }
        return false;
    });
    
    for (auto &oldTexture : oldTextures)
    {
        oldTexture.unusedFrameCount++;
    }
}

}

