#include "Function/UI/Text.h"
#include "Function/UI/DrawElementList.h"

namespace Zafkiel
{

uint32 Text::GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) 
{
    if (!wrap)
    {
        float yOffset = 0.0f;
        for (auto &textLine : textLines)
        {
            drawElementList.AddText(layerId, std::wstring(textLine.lineText), font.get(), fontSize, allocatedGeometry.position + vec2(0.0f, yOffset), color);
            yOffset += fontSize * 1.5f; // TODO: 目前固定1.5倍行宽
        }
        return layerId;
    }
    else
    {
        Log::Error("wrapping not implemented yet!");
        return layerId;
    }
    return layerId;
}


vec2 Text::GetDesiredSize() const 
{
    if (!wrap)
    {
        // TODO: cache
        float maxWidth = 0.0f;
        for (auto &textLine : textLines)
        {
            maxWidth = std::max(maxWidth, CalculateLineWidth(textLine.lineText, font, fontSize));
        }
        float height = textLines.size() * fontSize * 1.5f; // TODO: 目前固定1.5倍行宽
        return vec2(maxWidth, height);
    }
    else
    {
        Log::Error("wrapping not implemented yet!");
        return vec2(0.0f, 0.0f);
    }
}

float Text::CalculateLineWidth(std::wstring_view textLine, const Ref<Font> &font, float fontSize) const
{
    float lineWidth = 0;
    for (uint32 i = 0; i < textLine.length(); i++)
    {
        if (i != textLine.length() - 1)
        {
            wchar ch = textLine[i], nextCh = textLine[i + 1];
            float advance = font->GetGlyphData(ch)->advance + font->GetKerning(ch, nextCh);
            lineWidth += advance * fontSize;
        }
        else
        {
            wchar ch = textLine[i];
            float charWidth = font->GetGlyphData(ch)->planeCoordMax.x - font->GetGlyphData(ch)->planeCoordMin.x;
            lineWidth += charWidth * fontSize;
        }
    }
    return lineWidth;
}

void Text::UpdateTextLinesIfDirty()
{
    if (dirty)
    {
        dirty = false;

        textLines.clear();

        uint32 lineBegin = 0;
    
        for (uint32 i = 0; i < text.length(); i++)
        {
            wchar ch = text[i];
            
            if (ch == '\n')
            {
                std::wstring_view lineText(&text[lineBegin], i - lineBegin);
                textLines.emplace_back(lineText);
                lineBegin = i + 1;
            }
        }
        
        if (lineBegin <= text.length())
        {
            std::wstring_view lineText(&text[lineBegin], text.length() - lineBegin);
            textLines.emplace_back(lineText);
        }
    }
}

}