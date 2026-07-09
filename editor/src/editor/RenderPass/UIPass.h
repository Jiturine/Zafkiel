#pragma once
#include "Function/RHI/RHIResources.h"
#include "Function/Render/Font.h"
#include "Function/UI/DrawElementList.h"
#include "Function/UI/Window.h"

namespace Zafkiel
{

class PostProcessingPass;

class DrawElementPipeline
{
  public:
    virtual void StartBatch() = 0;

    void EndBatch() { Flush(); }

  protected:
    virtual void Flush() = 0;

    void FlushAndReset()
    {
        Flush();
        StartBatch();
    }
};

class QuadPipeline : public DrawElementPipeline
{
  public:
    QuadPipeline();

    ~QuadPipeline();

    struct QuadProps
    {
        vec3 position = vec3(0.0f);
        vec2 size = vec2(1.0f);
        vec4 color = vec4(1.0f);
        mat4 transform = mat4(1.0f);
        RHITexture *texture = nullptr;
        float rotation = 0.0f;
        float tilingFactor = 1.0f;
    };
    virtual void Flush() override;

    virtual void StartBatch() override;

    void DrawQuad(const QuadProps &props);

    void Resize(uint32 width, uint32 height);

    void ResetVertexBuffers();
    
    // 批处理绘制矩形
    struct QuadVertex
    {
        vec3 position;
        vec4 color;
        vec2 texCoord;
        int texIndex;
        uint32 entityID;
    };

    static constexpr uint32 maxVertexBuffers = 16; // TODO: 应该用fence/RDG 管理VBO同步
    Ref<RHIBuffer> quadVertexBuffer[maxVertexBuffers];
    uint32 currentVertexBufferIndex;

    Ref<RHIBuffer> quadIndexBuffer;
  
    Ref<RHIVertexShader> quadVertexShader;
    Ref<RHIFragmentShader> quadFragmentShader;

    Ref<RHITexture> whiteTexture;
    Ref<RHIGraphicsPipeline> pipeline;

    Ref<UniformBufferContent> quadUniformBufferContent;

    static constexpr uint32 maxQuads = 10000;
    static constexpr uint32 maxVertices = maxQuads * 4;
    static constexpr uint32 maxIndices = maxQuads * 6;
    static constexpr uint32 maxTextureSlots = 16;

    Ref<RHITexture> textureSlots[maxTextureSlots];
    uint32 textureSlotIndex = 1;
    QuadVertex *quadVertexBufferBase = nullptr;
    QuadVertex *quadVertexBufferPtr = nullptr;
    uint32 quadIndexCount = 0;
    static constexpr vec4 quadVertexPositions[]
    {
        {-0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f},
    };
    static constexpr vec2 quadTexCoords[]
    {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
    };
};
class TextPipeline : public DrawElementPipeline
{
  public:
    TextPipeline();
    ~TextPipeline();
    void DrawString(const std::wstring &str, Ref<Font> font, float fontSize, const vec2 &pos, const vec3 &color);
    virtual void Flush();
    virtual void StartBatch();

  private:
    struct TextVertex
    {
        vec3 position;
        vec4 color;
        vec2 texCoord;
        int texIndex;
        uint32 entityID;
    };
    Ref<RHIBuffer> textVertexBuffer;
    Ref<RHIBuffer> textIndexBuffer;

    Ref<RHIBuffer> textUniformBuffer;
    Ref<UniformBufferContent> textUniformBufferContent;

    Ref<RHITexture> whiteTexture;
  
    Ref<RHIVertexShader> vertexShader;
    Ref<RHIFragmentShader> fragmentShader;

    Ref<RHIGraphicsPipeline> pipeline;
    static constexpr uint32 maxChars = 10000;  // TODO: 优化，弹性大小，起始大小较小，超出时再分配内存
    static constexpr uint32 maxVertices = maxChars * 4;
    static constexpr uint32 maxIndices = maxChars * 6;
    static constexpr uint32 maxTextureSlots = 8;
    Ref<RHITexture> textureSlots[maxTextureSlots];

    Ref<Font> currentFont;

    uint32 textureSlotIndex = 1;
    TextVertex *textVertexBufferBase = nullptr;
    TextVertex *textVertexBufferPtr = nullptr;
    uint32 textIndexCount = 0;
};

class UIPass
{
  public:
    UIPass();

    void Render(const std::vector<Ref<Window>> &windows);

    void RenderSingleWindow(Ref<Window> window);

    Scope<QuadPipeline> quadPipeline;
    Scope<TextPipeline> textPipeline;

    Scope<DrawElementList> drawElementList;

    Ref<RHIBuffer> uiUniformBuffer;
    Ref<UniformBufferContent> uiUniformBufferContent;

}; 

}
