#pragma once
#include "Function/Render/Font.h"
#include "Function/UI/Widget.h"

namespace Zafkiel
{

struct TextLine
{
    std::wstring_view lineText;
};

class Text : public Widget
{
  public:
    void Construct(const std::wstring &text, Ref<Font> font, float fontSize, vec3 color, float wrapWidth) 
    {
        this->text = text;
        this->font = font;
        this->fontSize = fontSize;
        this->color = color;
        this->wrapWidth = wrapWidth;
        dirty = true;
        UpdateTextLinesIfDirty();
    }

    void SetText(const std::wstring &str)
    {
        dirty = true;
        text = str;
    }

    virtual uint32 GenerateDrawElements(WidgetGeometry allocatedGeometry, DrawElementList &drawElementList, WidgetGeometryList &widgetGeometryList, uint32 layerId) override;
    
    virtual vec2 GetDesiredSize() const override;

  private:
    float CalculateLineWidth(std::wstring_view textLine, const Ref<Font> &font, float fontSize) const;

    void UpdateTextLinesIfDirty();

    bool dirty;

    bool wrap = false;

    float width;

    float fontSize;

    Ref<Font> font;

    vec3 color = vec3(0.0f, 0.0f, 0.0f);

    std::wstring text;

    std::vector<TextLine> textLines;
};

}