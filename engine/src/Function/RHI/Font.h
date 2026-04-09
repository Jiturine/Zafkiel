#pragma once
#include "Function/RHI/RHIResources.h"

namespace Zafkiel 
{

class Font 
{
  public:
    Font(Ref<RHITexture> fontAtlasTexture) : fontAtlasTexture(fontAtlasTexture) {}

    RHITexture *GetFontAtlasTexture() { return fontAtlasTexture.get(); }

  private:
    Ref<RHITexture> fontAtlasTexture;
};

}
