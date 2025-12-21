#pragma once
#include "function/render/renderer2D.h"

namespace Zafkiel
{

#if 0

class EditorRenderer2D : public Renderer2D
{
  public:
    struct QuadProps
    {
        vec3 position = vec3(0.0f);
        vec2 size = vec2(1.0f);
        vec4 color = vec4(1.0f);
        mat4 transform = mat4(1.0f);
        Scope<Texture2D> texture = nullptr;
        float rotation = 0.0f;

        float tilingFactor = 1.0f;
        uint32_t entityID = entt::null;
    };

    
    void BeginScene(const mat4 &viewProjectionMatrix);
    void EndScene();
    void Flush();
    void StartBatch();
    void FlushAndReset();
    void DrawQuad(const QuadProps &props);
    
    static void Init()
    {
        instance.reset(new EditorRenderer2D);
    }
    static void Destroy()
    {
        instance = nullptr;
    }
    static EditorRenderer2D& Instance() { return instance.As<EditorRenderer2D>(); }
    
  private:
    EditorRenderer2D();
    // 批处理绘制矩形
    struct QuadVertex
    {
        vec3 position;
        vec4 color;
        vec2 texCoord;
        int texIndex;
        uint32_t entityID;
    };

    Ref<VertexBuffer> quadVertexBuffer;
    Ref<IndexBuffer> quadIndexBuffer;
    Ref<Shader> textureShader;
    Scope<Texture2D> whiteTexture;
    static constexpr uint32_t maxQuads = 10000;
    static constexpr uint32_t maxVertices = maxQuads * 4;
    static constexpr uint32_t maxIndices = maxQuads * 6;
    static constexpr uint32_t maxTextureSlots = 32;
    Scope<Texture2D> textureSlots[maxTextureSlots];
    uint32_t textureSlotIndex = 1;
    QuadVertex *quadVertexBufferBase = nullptr;
    QuadVertex *quadVertexBufferPtr = nullptr;
    uint32_t quadIndexCount = 0;
    static constexpr vec4 quadVertexPositions[] = {
        {-0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f}};
    static constexpr vec2 quadTexCoords[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}};
};
#endif 

}