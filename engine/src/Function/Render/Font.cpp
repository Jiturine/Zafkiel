#include "Function/Render/Font.h"
#include "Function/RHI/RHI.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdfgen.h>

namespace Zafkiel 
{

Font::Font(const Path &path)
{
    msdfgen::FreetypeHandle *library = msdfgen::initializeFreetype();
    if (!library)
    {
        Log::Error("Failed to initialize Freetype!");
        return;
    }

    msdfgen::FontHandle *font = msdfgen::loadFont(library, path.string().c_str());
    if (!font)
    {
        Log::Error("Failed to load font!");
        return;
    }

    std::vector<msdf_atlas::GlyphGeometry> glyphs;
    msdf_atlas::FontGeometry fontGeometry(&glyphs);
    
    int glyphNum = fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);
    Log::Info("charset num: {}", msdf_atlas::Charset::ASCII.size());
    Log::Info("glyph num: {}", glyphNum);
    
    msdf_atlas::TightAtlasPacker atlasPacker;
    atlasPacker.setPixelRange(pixelRange);
    atlasPacker.setMiterLimit(1.0);
    atlasPacker.setScale(40.0);
    int remaining = atlasPacker.pack(glyphs.data(), glyphs.size());
    Log::Info("remaining: {}", remaining);
    
    int width, height;
    atlasPacker.getDimensions(width, height);
    Log::Info("width: {}, height: {}", width, height);
    
    msdf_atlas::GeneratorAttributes attributes;
    attributes.config.overlapSupport = true;
    attributes.scanlinePass = true;

    for (auto& glyph : glyphs) 
    {
        glyph.edgeColoring(msdfgen::edgeColoringInkTrap, 3.0, 0);
    }

    msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<float, 3>> generator(width, height);
    generator.setAttributes(attributes);
    generator.setThreadCount(8);
    generator.generate(glyphs.data(), glyphs.size());

    msdfgen::BitmapConstRef<float, 3> bitmap = msdfgen::BitmapConstRef<float, 3>(generator.atlasStorage());

    // debug
    msdfgen::savePng(bitmap, "assets/fonts/export.png");

    msdfgen::destroyFont(font);
    msdfgen::deinitializeFreetype(library);

    std::vector<float> rgbaData;
    rgbaData.reserve(width * height * 4);

    uint32 srcIndex = 0;
    for (int y = height - 1; y >= 0; --y)
    {
        srcIndex = y * width * 3;
        for (int x = 0; x < width; ++x)
        {
            rgbaData.push_back(bitmap.pixels[srcIndex++]);
            rgbaData.push_back(bitmap.pixels[srcIndex++]);
            rgbaData.push_back(bitmap.pixels[srcIndex++]);
            rgbaData.push_back(0.0f);
        }
    }

    RHITextureDesc altasTextureDesc
    {
        .width = (uint32)width,
        .height = (uint32)height,
        .format = ImageFormat::RGBA32F,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::Upload,
        .initialLayout = ImageLayout::ShaderReadOnly,
        .filter = TextureFilter::Linear,
    };

    auto atlas = GlobalRHICmdList->CreateTexture(altasTextureDesc, Buffer(reinterpret_cast<const uint8 *>(rgbaData.data()), rgbaData.size() * sizeof(float)));
    fontAtlasTextures.push_back(atlas);

    auto &metrics = fontGeometry.getMetrics();
    lineHeight = metrics.ascenderY - metrics.descenderY;
    ascender = metrics.ascenderY;
    descender = metrics.descenderY;
    float fsScale = 1.0f / lineHeight;

    auto &msdfKernings = fontGeometry.getKerning();
    for (auto &[chars, kerning] : msdfKernings)
    {
        kernings[chars] = kerning * fsScale;
    }

    for (auto &glyph : glyphs)
    {
        double al, ab, ar, at;
        glyph.getQuadAtlasBounds(al, ab, ar, at);
        
        double pl, pb, pr, pt;
        glyph.getQuadPlaneBounds(pl, pb, pr, pt);

        // 注意：msdf-atlas-gen使用左下角为原点，ab=bottom, at=top
        vec2 atlasCoordMin(al, ab), atlasCoordMax(ar, at);
        vec2 planeCorrdMin(pl, pb), planeCoordMax(pr, pt);

        planeCorrdMin *= fsScale;  // 字体高度统一为1
        planeCoordMax *= fsScale;

        atlasCoordMin /= glm::vec2(width, height);
        atlasCoordMax /= glm::vec2(width, height);

        atlasCoordMin.y = 1 - atlasCoordMin.y;
        atlasCoordMax.y = 1 - atlasCoordMax.y;

        glyphDatas[glyph.getCodepoint()] = GlyphData {
            .codepoint = glyph.getCodepoint(),
            .advance = (float)(glyph.getAdvance() * fsScale),
            .atlasCoordMin = atlasCoordMin,
            .atlasCoordMax = atlasCoordMax,
            .planeCoordMin = planeCorrdMin,
            .planeCoordMax = planeCoordMax,
            .altasIndex = 0,
        };
    }
}

const GlyphData *Font::GetGlyphData(char32 ch) const
{
    if (glyphDatas.contains(ch))
        return &glyphDatas.at(ch);
    else 
        return nullptr;
}

float Font::GetKerning(char32 ch, char32 next) const
{
    if (kernings.contains({ch, next}))
        return kernings.at({ch, next});
    else 
        return 0.0f;
}

}