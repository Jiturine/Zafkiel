#include "Function/UI/DrawElementList.h"

namespace Zafkiel 
{

void DrawElementList::GenerateBatches()
{
    std::stable_sort(elements.begin(), elements.end(), [](auto &a, auto &b) {
        return a->layerId < b->layerId;
    });

    ElementBatch currentQuadBatch; currentQuadBatch.type = DrawElementType::Quad;
    ElementBatch currentTextBatch; currentTextBatch.type = DrawElementType::Text;

    for (auto &element : elements)
    {
        switch (element->type)
        {
            using enum DrawElementType;
        case Quad:
        {
            auto &quadElement = element.As<QuadElement>();
            if (quadElement.layerId == currentQuadBatch.layerId)
            {
                currentQuadBatch.elements.push_back(MoveTemp(element));
            }
            else
            {
                if (!currentQuadBatch.elements.empty())
                {
                    batches.push_back(MoveTemp(currentQuadBatch));
                    currentQuadBatch.elements.clear();
                }

                currentQuadBatch.layerId = element->layerId;
                currentQuadBatch.type = DrawElementType::Quad;
                currentQuadBatch.elements.push_back(MoveTemp(element));
            }
            break;
        }
        case Text:
        {
            auto &textElement = element.As<TextElement>();
            if (textElement.layerId == currentTextBatch.layerId)
            {
                currentTextBatch.elements.push_back(MoveTemp(element));
            }
            else
            {
                if (!currentTextBatch.elements.empty())
                {
                    batches.push_back(MoveTemp(currentTextBatch));
                    currentTextBatch.elements.clear();
                }

                currentTextBatch.layerId = element->layerId;
                currentTextBatch.type = DrawElementType::Text;
                currentTextBatch.elements.push_back(MoveTemp(element));
            }
            break;
        }
        }
    }

    if (!currentQuadBatch.elements.empty())
        batches.push_back(MoveTemp(currentQuadBatch));
    if (!currentTextBatch.elements.empty())
        batches.push_back(MoveTemp(currentTextBatch));

    // 对 batch再次排序，将同layer的相同类型的batch放在一起，减少pipeline切换
    std::stable_sort(batches.begin(), batches.end(), [](auto &a, auto &b) {
        return a.layerId < b.layerId || (a.layerId == b.layerId && (int)a.type < (int)b.type);
    });
}

}