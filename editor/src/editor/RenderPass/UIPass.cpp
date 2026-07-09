#include "editor/RenderPass/UIPass.h"
#include "editor/RenderPass/PostProcessingPass.h"
#include "Function/Render/Renderer.h"

namespace Zafkiel
{
QuadPipeline::QuadPipeline()
{
    quadVertexBufferBase = new QuadVertex[maxVertices];

    RHIBufferDesc quadVertexBufferDesc
    {
        .size = maxVertices * sizeof(QuadVertex),
        .usages = BufferUsageFlags::VertexBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Dynamic,
    };
    for (uint32 i = 0; i < maxVertexBuffers; i++)
        quadVertexBuffer[i] = GlobalRHICmdList->CreateBuffer(quadVertexBufferDesc);
    
    std::vector<uint32> indices(maxIndices);
    for (uint32 i = 0, offset = 0; i < maxIndices; i += 6, offset += 4)
    {
        indices[i] = offset;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset;
    }

    RHIBufferDesc quadIndexBufferDesc
    {
        .size = maxIndices * sizeof(uint32),
        .usages = BufferUsageFlags::IndexBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Static,
    };
    quadIndexBuffer = GlobalRHICmdList->CreateBuffer(quadIndexBufferDesc, indices.data());

    RHITextureDesc desc
    {
        .width = 1,
        .height = 1,
        .format = ImageFormat::RGBA8,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::Upload,
        .initialLayout = ImageLayout::ShaderReadOnly,
    };
    uint32 whiteTextureData = 0xffffffff;
    Buffer buffer(whiteTextureData);
    whiteTexture = GlobalRHICmdList->CreateTexture(desc, buffer);

    quadVertexShader = GlobalRHI->CreateVertexShader("assets/shaders/QuadVS.glsl");
    quadFragmentShader = GlobalRHI->CreateFragmentShader("assets/shaders/QuadFS.glsl");

    RHIGraphicsPipelineDesc pipelineDesc
    {
        .shaders = {quadVertexShader.get(), quadFragmentShader.get()},
        .primitiveTopology = PrimitiveTopology::Triangles,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .depthTest = false,
        .renderTargetDesc
        {
            .colorAttachmentDescs
            {
                {
                    .format = ImageFormat::RGBA8,
                    .blendEnable = true,
                    .blendFunc = BlendFunc::Normal,
                    .sampleCount = 1,
                }
            }
        }
    };
    pipeline = GlobalRHI->CreateGraphicsPipeline(pipelineDesc);
}

QuadPipeline::~QuadPipeline()
{
    delete[] quadVertexBufferBase;
    quadVertexBufferBase = nullptr;
    quadVertexBufferPtr = nullptr;
}

void QuadPipeline::ResetVertexBuffers()
{
    currentVertexBufferIndex = 0;
}

void QuadPipeline::Flush()
{
    if (quadIndexCount == 0)
        return;

    uint32 size = (quadVertexBufferPtr - quadVertexBufferBase) * sizeof(QuadVertex);
    GlobalRHICmdList->WriteBuffer(quadVertexBuffer[currentVertexBufferIndex].get(), Buffer(reinterpret_cast<uint8 *>(quadVertexBufferBase), size));

    for (uint32 index = 0; index < maxTextureSlots; index++)
    {
        pipeline->SetTexture(ShaderStage::Fragment, std::format("uTexture{}", index), textureSlots[index].get());
    }

    GlobalRHICmdList->BindGraphicsPipeline(pipeline.get());

    GlobalRHICmdList->DrawIndexed(quadVertexBuffer[currentVertexBufferIndex].get(), quadIndexBuffer.get(), quadIndexCount);

    currentVertexBufferIndex++;
    if (currentVertexBufferIndex >= maxVertexBuffers)
    {
        Log::Warn("VBO Overflow!");
    }
}

void QuadPipeline::StartBatch()
{
    for (auto index = 0; index < maxTextureSlots; index++)
    {
        textureSlots[index] = whiteTexture;
    }
    quadVertexBufferPtr = quadVertexBufferBase;
    quadIndexCount = 0;
    textureSlotIndex = 1;
}

void QuadPipeline::DrawQuad(const QuadProps &props)
{
    if (quadIndexCount >= maxIndices)
        FlushAndReset();

    uint32 textureIndex = 0;
    if (props.texture)
    {
        for (uint32 i = 1; i < textureSlotIndex; ++i)
        {
            if (textureSlots[i] == props.texture)
            {
                textureIndex = i;
                break;
            }
        }

        if (textureIndex == 0)
        {
            if (textureSlotIndex >= maxTextureSlots)
            {
                FlushAndReset();
            }
            textureIndex = textureSlotIndex;
            textureSlots[textureSlotIndex] = props.texture;
            textureSlotIndex++;
        }
    }

    glm::mat4 transform;
    if (props.transform == glm::mat4(1.0f))
    {
        transform = glm::translate(glm::mat4(1.0f), props.position) *
            glm::rotate(glm::mat4(1.0f), props.rotation, {0.0f, 0.0f, 1.0f}) *
            glm::scale(glm::mat4(1.0f), {props.size.x, props.size.y, 1.0f});
    }
    else
    {
        transform = props.transform;
    }

    for (int i = 0; i < 4; i++)
    {
        quadVertexBufferPtr->position = transform * quadVertexPositions[i];
        quadVertexBufferPtr->color = props.color;
        quadVertexBufferPtr->texCoord = quadTexCoords[i];
        quadVertexBufferPtr->texIndex = textureIndex;
        quadVertexBufferPtr->entityID = 0;
        quadVertexBufferPtr++;
    }
    quadIndexCount += 6;
} 

TextPipeline::TextPipeline()
{
    textVertexBufferBase = new TextVertex[maxVertices];

    RHIBufferDesc textVertexBufferDesc
    {
        .size = maxVertices * sizeof(TextVertex),
        .usages = BufferUsageFlags::VertexBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Dynamic,
    };
    textVertexBuffer = GlobalRHICmdList->CreateBuffer(textVertexBufferDesc);
    
    std::vector<uint32> indices(maxIndices);
    for (uint32 i = 0, offset = 0; i < maxIndices; i += 6, offset += 4)
    {
        indices[i] = offset;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset;
    }

    RHIBufferDesc textIndexBufferDesc
    {
        .size = maxIndices * sizeof(uint32),
        .usages = BufferUsageFlags::IndexBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Static,
    };
    textIndexBuffer = GlobalRHICmdList->CreateBuffer(textIndexBufferDesc, indices.data());

    vertexShader = GlobalRHI->CreateVertexShader("assets/shaders/TextVS.glsl");
    fragmentShader = GlobalRHI->CreateFragmentShader("assets/shaders/TextFS.glsl");

    RHITextureDesc desc
    {
        .width = 1,
        .height = 1,
        .format = ImageFormat::RGBA8,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::Upload,
        .initialLayout = ImageLayout::ShaderReadOnly,
    };
    uint32 whiteTextureData = 0xffffffff;
    Buffer buffer(whiteTextureData);
    whiteTexture = GlobalRHICmdList->CreateTexture(desc, buffer);

    RHIGraphicsPipelineDesc pipelineDesc
    {
        .shaders = {vertexShader.get(), fragmentShader.get()},
        .primitiveTopology = PrimitiveTopology::Triangles,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .depthTest = false,
        .renderTargetDesc
        {
            .colorAttachmentDescs
            {
                {
                    .format = ImageFormat::RGBA8,
                    .blendEnable = true,
                    .blendFunc = BlendFunc::Normal,
                    .sampleCount = 1,
                }
            }
        }
    };
    pipeline = GlobalRHI->CreateGraphicsPipeline(pipelineDesc);

    auto textUBOType = fragmentShader->GetResourceTable().GetResourceType("TextUBO")->As<ShaderReflection::UniformBlock>();
    RHIBufferDesc textUniformBufferDesc
    {
        .size = textUBOType->GetSize(),
        .usages = BufferUsageFlags::UniformBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Dynamic,
    };
    textUniformBuffer = GlobalRHICmdList->CreateBuffer(textUniformBufferDesc);
    textUniformBufferContent = CreateRef<UniformBufferContent>(textUBOType);

    pipeline->SetUniformBuffer(ShaderStage::Fragment, "uText", textUniformBuffer.get());
}

TextPipeline::~TextPipeline()
{
    delete[] textVertexBufferBase;
    textVertexBufferBase = nullptr;
    textVertexBufferPtr = nullptr;
}

void TextPipeline::DrawString(const std::wstring &str, Ref<Font> font, float fontSize, const vec2 &pos, const vec3 &color)
{
    Ref<RHITexture> altas = font->GetFontAtlasTexture(0);

    float xOffset = 0.0f;
    
    float fsScale = 1.0f / font->GetLineHeight();

    float baselineY = pos.y + font->GetAscender() * fontSize;

    for (uint32 i = 0; i < str.length(); i++)
    {
        wchar ch = str[i];
        auto glyphData = font->GetGlyphData(ch);
        if (!glyphData)
            glyphData = font->GetGlyphData('?');
        if (!glyphData)
        {
            Log::Error("Invalid Character in Font!");
            return;
        }

        if (font != currentFont)
        {
            FlushAndReset();
            currentFont = font;
        }

        textureSlots[glyphData->altasIndex] = font->GetFontAtlasTexture(glyphData->altasIndex);

        vec4 charVertexPositions[4] =
        {
            vec4(glyphData->planeCoordMin, 0.0f, 1.0f), 
            vec4(glyphData->planeCoordMax.x, glyphData->planeCoordMin.y, 0.0f, 1.0f),
            vec4(glyphData->planeCoordMax, 0.0f, 1.0f),
            vec4(glyphData->planeCoordMin.x, glyphData->planeCoordMax.y, 0.0f, 1.0f), 
        };

        vec2 charTexCoords[4] =
        {
            glyphData->atlasCoordMin,
            vec2(glyphData->atlasCoordMax.x, glyphData->atlasCoordMin.y),
            glyphData->atlasCoordMax,
            vec2(glyphData->atlasCoordMin.x, glyphData->atlasCoordMax.y),
        };

        for (int i = 0; i < 4; i++)
        {
            vec3 planeCoordPos = glm::scale(mat4(1.0f), vec3(fontSize, fontSize, 1.0f)) * charVertexPositions[i];
            textVertexBufferPtr->position = vec3(pos.x + xOffset + planeCoordPos.x, baselineY - planeCoordPos.y, 0.0f);
            textVertexBufferPtr->color = vec4(color, 1.0f);
            textVertexBufferPtr->texCoord = charTexCoords[i];
            textVertexBufferPtr->texIndex = glyphData->altasIndex;
            textVertexBufferPtr->entityID = 0;
            textVertexBufferPtr++;
        }
        textIndexCount += 6;

        if (i != str.length() - 1)
        {
            wchar nextCh = str[i + 1];
            float advance = font->GetGlyphData(ch)->advance + font->GetKerning(ch, nextCh);
            xOffset += advance * fontSize;
        }
        else
        {
            float charWidth = font->GetGlyphData(ch)->planeCoordMax.x - font->GetGlyphData(ch)->planeCoordMin.x;
            xOffset += charWidth * fontSize;
        }
    }
}

void TextPipeline::Flush()
{
    if (textIndexCount == 0)
    {
        return;
    }

    uint32 size = (textVertexBufferPtr - textVertexBufferBase) * sizeof(TextVertex);

    GlobalRHICmdList->WriteBuffer(textVertexBuffer.get(), Buffer(reinterpret_cast<uint8 *>(textVertexBufferBase), size));

    if (currentFont)
    {
        textUniformBufferContent->SetParameter("pxRange", currentFont->GetPixelRange(), ShaderFundamentalType::Float);
    }

    GlobalRHICmdList->UpdateUniformBuffer(textUniformBuffer.get(), textUniformBufferContent->GetData());

    for (uint32 index = 0; index < maxTextureSlots; index++)
    {
        pipeline->SetTexture(ShaderStage::Fragment, std::format("uTexture{}", index), textureSlots[index].get());
    }

    GlobalRHICmdList->BindGraphicsPipeline(pipeline.get());

    GlobalRHICmdList->DrawIndexed(textVertexBuffer.get(), textIndexBuffer.get(), textIndexCount);
}

void TextPipeline::StartBatch()
{
    currentFont = nullptr;  // 重置当前字体，避免使用失效指针
    for (auto index = 0; index < maxTextureSlots; index++)
    {
        textureSlots[index] = whiteTexture;
    }
    textVertexBufferPtr = textVertexBufferBase;
    textIndexCount = 0;
}

UIPass::UIPass()
{
    quadPipeline = CreateScope<QuadPipeline>();

    textPipeline = CreateScope<TextPipeline>();

    auto uiuboType = quadPipeline->quadVertexShader->GetResourceTable().GetResourceType("UIUBO")->As<ShaderReflection::UniformBlock>();

    RHIBufferDesc quadUniformBufferDesc 
    {
        .size = uiuboType->GetSize(),
        .usages = BufferUsageFlags::UniformBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Dynamic,
    };
    uiUniformBuffer = GlobalRHICmdList->CreateBuffer(quadUniformBufferDesc);

    GlobalRHICmdList->SetStaticUniformBuffer("uUIUBO", uiUniformBuffer.get());

    uiUniformBufferContent = CreateRef<UniformBufferContent>(uiuboType);

    drawElementList = CreateScope<DrawElementList>();
}

void UIPass::Render(const std::vector<Ref<Window>> &windows)
{
    quadPipeline->ResetVertexBuffers();

    for (auto window : windows)
    {
        RenderSingleWindow(window);

        GlobalRHICmdList->Present(window->GetViewport());
    }
}

void UIPass::RenderSingleWindow(Ref<Window> window)
{
    RHIRenderPassInfo beginInfo
    {
        .colorAttachments
        {
            {
                .texture = window->GetViewport()->GetBackendTexture(),
                .clearValue = { .vec4Value = vec4(0, 0, 0, 1) },
                .initialLayout = ImageLayout::Undefined,
                .finalLayout = ImageLayout::PresentSrc
            }
        }
    };
    GlobalRHICmdList->BeginRenderPass(beginInfo);

    mat4 viewMatrix = Maths::LookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 orthoProjMatrix = Maths::Ortho(0, window->GetWidth(), 0, window->GetHeight(), 0.1f, 100.0f);
    mat4 viewProj = orthoProjMatrix * viewMatrix;

    uiUniformBufferContent->SetParameter("viewProjection", viewProj, ShaderFundamentalType::Mat4);

    GlobalRHICmdList->UpdateUniformBuffer(uiUniformBuffer.get(), uiUniformBufferContent->GetData());
    
    std::vector<DrawElementList> lists;
    window->DrawWindow(lists);
    
    for (auto &drawElementList : lists)
    {
        drawElementList.GenerateBatches();

        for (auto &batch : drawElementList.GetBatches())
        {
            switch (batch.type)
            {
                using enum DrawElementList::DrawElementType;
            case Quad:
            {
                quadPipeline->StartBatch();
                for (auto &element : batch.elements)
                {
                    auto &quadElement = element.As<DrawElementList::QuadElement>();
                    quadPipeline->DrawQuad({
                        .position = vec3((quadElement.quadPosMin + quadElement.quadPosMax) / 2.0f, 0.0f),
                        .size = (quadElement.quadPosMax - quadElement.quadPosMin),
                        .color = quadElement.color,
                        .texture = quadElement.texture,
                    });
                }
                quadPipeline->EndBatch();
                break;
            }
            case Text:
            {
                textPipeline->StartBatch();
                for (auto &element : batch.elements)
                {
                    auto &textElement = element.As<DrawElementList::TextElement>();
                    textPipeline->DrawString(
                        textElement.str,
                        textElement.font,
                        textElement.fontSize,
                        textElement.pos,
                        textElement.color
                    );
                }
                textPipeline->EndBatch();
                break;
            }
            }
        }
    }
    GlobalRHICmdList->EndRenderPass();
}

}
