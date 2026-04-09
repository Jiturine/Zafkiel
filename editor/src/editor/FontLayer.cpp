#include "editor/FontLayer.h"
#include "Core/Application/Application.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdfgen.h>

namespace Zafkiel 
{

void FontLayer::OnAttach() 
{
    msdfgen::FreetypeHandle *library = msdfgen::initializeFreetype();
    if (!library)
    {
        Log::Error("Failed to initialize Freetype!");
        return;
    }

    msdfgen::FontHandle *font = msdfgen::loadFont(library, "/home/jiturine/Workspace/Zafkiel/sandbox/assets/fonts/JetBrainsMonoNerdFont-Regular.ttf");
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
    atlasPacker.setPixelRange(2.0);
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

    msdfgen::savePng(bitmap, "output_atlas.png");

    msdfgen::destroyFont(font);            
    msdfgen::deinitializeFreetype(library);

}

void FontLayer::OnDetach()
{

}

void FontLayer::OnUpdate(float timestep)
{
    Application::Instance().Exit();
}

}