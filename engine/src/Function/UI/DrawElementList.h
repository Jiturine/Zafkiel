#pragma once
#include "Function/RHI/RHIResources.h"
#include "Function/Render/Font.h"

namespace Zafkiel
{

class DrawElementList
{
  public:
    void Clear()
    {
        elements.clear();
    }

    void AddQuad(uint32 layerId, vec2 quadPosMin, vec2 quadPosMax, vec4 color, RHITexture *texture = nullptr)
    {
        elements.push_back(CreateScope<QuadElement>(layerId, quadPosMin, quadPosMax, color, texture));
    }

    void AddText(uint32 layerId, const std::wstring &str, Font *font, float fontSize, vec2 pos, vec3 color)
    {
        elements.push_back(CreateScope<TextElement>(layerId, str, font, fontSize, pos, color));
    }

    enum DrawElementType
    {
        Quad, 
        Text,
    };
    
    struct DrawElement
    {
        DrawElement(DrawElementType type, uint32 layerId) : type(type), layerId(layerId) {}

        DrawElementType type;

        uint32 layerId;
    };

    struct QuadElement : public DrawElement
    {
        QuadElement(uint32 layerId, vec2 quadPosMin, vec2 quadPosMax, vec4 color, RHITexture *texture) 
            : DrawElement(DrawElementType::Quad, layerId), quadPosMin(quadPosMin), quadPosMax(quadPosMax),
              color(color), texture(texture) {}

        vec2 quadPosMin;
        vec2 quadPosMax;
        vec4 color;
        RHITexture *texture;
    };

    struct TextElement : public DrawElement
    {
        TextElement(uint32 layerId, const std::wstring &str, Font *font, float fontSize, vec2 pos, vec3 color)
            : DrawElement(DrawElementType::Text, layerId), str(str), font(font), fontSize(fontSize), pos(pos), color(color) {}

        std::wstring str;
        Font *font;
        float fontSize;
        vec2 pos;
        vec3 color;
    };
    
    struct ElementBatch
    {
        uint32 layerId;
        DrawElementType type;
        std::vector<Scope<DrawElement>> elements;
    };

  private:
    std::vector<Scope<DrawElement>> elements;

    std::vector<ElementBatch> batches;

  public:
    void GenerateBatches();

    const std::vector<ElementBatch> &GetBatches() const { return batches; }

};

}