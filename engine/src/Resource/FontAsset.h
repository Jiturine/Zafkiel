#pragma once
#include "Resource/Asset.h"

namespace Zafkiel 
{

struct GlyphData
{
    char32 codepoint;
    float advance;
    vec2 atlasCoordMin; // [0, 1] 左上角(0, 0)
    vec2 atlasCoordMax; // [0, 1]
    vec2 planeCoordMin; // baseline起点 为原点
    vec2 planeCoordMax;
    uint32 altasIndex;
};

class FontAsset : public Asset
{
  public:
    FontAsset(AssetHandle handle) : Asset(handle) {}
    virtual AssetType GetAssetType() const override { return AssetType::Font; }

    struct FontAtlasData 
    {
        ScopedBuffer data;
        uint32 Width;
        uint32 Height;
    };
    
    std::vector<FontAtlasData> fontAtlasData;

    const FontAtlasData &GetFontAtlasData(uint32 index) const { return fontAtlasData[index]; }

    uint32 fontHeight;
    std::vector<GlyphData> glyphDatas;
};

}