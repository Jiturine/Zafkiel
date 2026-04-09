#pragma once
#include "Resource/Asset.h"

namespace Zafkiel 
{

struct GlyphData
{
    uint32 codepoint;
    float advance;
    vec4 uv;
    vec2 bearing;
};

class FontAsset : public Asset
{
  public:
    FontAsset(AssetHandle handle) : Asset(handle) {}
    virtual AssetType GetAssetType() const override { return AssetType::Font; }

    Buffer GetFontAtlasData() const { return fontAtlasData; }
    uint32 GetAtlasWidth() const { return atlasWidth; }
    uint32 GetAtlasHeight() const { return atlasHeight; }
    
    ScopedBuffer fontAtlasData;
    uint32 atlasWidth;
    uint32 atlasHeight;
    uint32 fontHeight;
    std::vector<GlyphData> glyphData;
};

}