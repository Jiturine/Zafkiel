#include "editor/Resource/AssetImporters/FontImporter.h"
#include "editor/Resource/EditorAssetManager.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdfgen.h>
#include <msdfgen/ext/import-font.h>

namespace Zafkiel 
{

Ref<EditorAssetMetadata> FontImporter::Import(const Path &assetPath)
{
    AssetHandle fontHandle;

    msdfgen::FreetypeHandle *library = msdfgen::initializeFreetype();

    if (!library)
    {
        Log::Error("Failed to initialize Freetype!");
        return nullptr;
    }
    msdfgen::FontHandle *font = msdfgen::loadFont(library, (EditorAssetManager::Instance().GetAssetDirectory() / assetPath).string().c_str());
    if (!font)
    {
        Log::Error("Failed to load font!");
        return nullptr;
    }

    std::vector<msdf_atlas::GlyphGeometry> glyphs;
    msdf_atlas::FontGeometry fontGeometry(&glyphs);
    
    fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);

    auto &metrics = fontGeometry.getMetrics();
    float fontHeight = metrics.ascenderY - metrics.descenderY;
    float fsScale = 1.0f / fontHeight;

    
    
    msdf_atlas::TightAtlasPacker atlasPacker;
    atlasPacker.setPixelRange(2.0);
    atlasPacker.setMiterLimit(1.0);
    atlasPacker.setScale(40.0);
    atlasPacker.pack(glyphs.data(), glyphs.size());
    
    int width, height;
    atlasPacker.getDimensions(width, height);
    
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

    Path fontAtlasPath = assetPath;
    fontAtlasPath.replace_filename(std::format("{}_atlas.png", assetPath.filename().stem().string()));
    msdfgen::savePng(bitmap, (EditorAssetManager::Instance().GetAssetDirectory() / fontAtlasPath).string().c_str());
    
    Path glyphDataPath = assetPath;
    fontAtlasPath.replace_filename(std::format("{}_glyph_data.bin", assetPath.filename().stem().string()));
    
    
    Path generatedAssetPath = assetPath;
    generatedAssetPath.replace_extension(".zff");
    
    YamlSerializer serializer;
    serializer.BeginMap();
    serializer.Key("FontAtlasPath").Value(fontAtlasPath);
    serializer.Key("GlyphData").Value(glyphDataPath);
    serializer.EndMap();
    FileSystem::SaveText(EditorAssetManager::Instance().GetAssetDirectory() / generatedAssetPath, serializer.c_str());

    auto fontMetadata = CreateRef<EditorAssetMetadata>(fontHandle, AssetType::Font, assetPath);
    return fontMetadata;
}

}