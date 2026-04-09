#pragma once
#include "Function/RHI/RHIResources.h"

namespace Zafkiel
{

class PostProcessingPass;

class QuadPipeline
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
        Ref<RHITexture> texture = nullptr;
        float rotation = 0.0f;
        float tilingFactor = 1.0f;
    };
    void Render(uint32 width, uint32 height);
    void BeginScene(const mat4 &viewProjectionMatrix);
    void EndScene();
    void Flush();
    void StartBatch();
    void FlushAndReset();
    void DrawQuad(const QuadProps &props);
    void Resize(uint32 width, uint32 height);
    
  private:
    // 批处理绘制矩形
    struct QuadVertex
    {
        vec3 position;
        vec4 color;
        vec2 texCoord;
        int texIndex;
        uint32 entityID;
    };

    Ref<RHIBuffer> quadVertexBuffer;
    Ref<RHIBuffer> quadIndexBuffer;
  
    Ref<RHIVertexShader> quadVertexShader;
    Ref<RHIFragmentShader> quadFragmentShader;

    Ref<RHITexture> whiteTexture;
    Ref<RHIGraphicsPipeline> pipeline;

    Ref<RHIBuffer> quadUniformBuffer;
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
#if 0
class TextPipeline
{
  public:
    TextPipeline();
    ~TextPipeline();
    // void DrawString(const std::string &str, Ref<FontAsset> font, const mat4 &transform, const vec3 &color);
    
  private:
    struct TextVertex
    {
        vec3 position;
        vec4 color;
        vec2 texCoord;
        uint32 entityID;
    };
    Ref<RHIBuffer> textVertexBuffer;
    Ref<RHIBuffer> textIndexBuffer;
    Ref<RHIGraphicsShader> textShader;
    Ref<RHIGraphicsPipeline> pipeline;
    static constexpr uint32 maxChars = 10000;  // TODO: 优化，弹性大小，起始大小较小，超出时再分配内存
    static constexpr uint32 maxVertices = maxChars * 4;
    static constexpr uint32 maxIndices = maxChars * 6;
    Ref<RHITexture> textureSlot;

    uint32 textureSlotIndex = 1;
    TextVertex *textVertexBufferBase = nullptr;
    TextVertex *textVertexBufferPtr = nullptr;
    uint32 textIndexCount = 0;
    static constexpr vec4 textVertexPositions[]
    {
        {-0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f},
    };
    static constexpr vec2 textTexCoords[]
    {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
    };
    
};
#endif 

class UICompositePipeline
{
  public:
    UICompositePipeline(PostProcessingPass *postProcessingPass);
    void Render();
    void Resize();
    Ref<RHIBuffer> vertexBuffer;
    Ref<RHIBuffer> indexBuffer;
  
    Ref<RHIGraphicsShader> vertexShader;
    Ref<RHIGraphicsShader> fragmentShader;
    
    Ref<RHIGraphicsPipeline> pipeline;

    PostProcessingPass *postProcessingPass;
};

class UIPass
{
  public:
    UIPass(PostProcessingPass *postProcessingPass);
    void Render();
    void Resize(uint32 width, uint32 height);

    Ref<RHITexture> outputTexture;
  
    mat4 lightViewProj;
    Scope<QuadPipeline> quadPipeline;
    Scope<UICompositePipeline> uiCompositePipeline;

    PostProcessingPass *postProcessingPass;

    uint32 currentWidth = 1;
    uint32 currentHeight = 1;

}; 

}
