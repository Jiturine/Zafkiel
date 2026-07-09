#pragma once
#include "Function/RHI/RHIResources.h"
#include "Platform/Filesystem/Filesystem.h"
#include "Resource/FontAsset.h"

namespace Zafkiel 
{

class Font : public RefCounted
{
  public:
    Font(const Path &path);

    RHITexture *GetFontAtlasTexture(uint32 index) { return fontAtlasTextures[index].get(); }

    const GlyphData *GetGlyphData(char32 ch) const;

    float GetKerning(char32 ch, char32 next) const;

    uint32 GetLineHeight() const { return lineHeight; }

    float GetPixelRange() const { return pixelRange; }

    float GetAscender() const { return ascender; }
    float GetDescender() const { return descender; }

  private:
    std::vector<Ref<RHITexture>> fontAtlasTextures;

    std::unordered_map<char32, GlyphData> glyphDatas;

    std::map<std::pair<char32, char32>, float> kernings;

    float pixelRange = 2.0f;

    float lineHeight;

    float ascender = 0.0f;
    float descender = 0.0f;
};

}
