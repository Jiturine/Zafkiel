#pragma once
#include "function/render/renderer2D.h"

namespace Zafkiel
{

class EditorRenderer2D : public Renderer2D
{
  public:
    struct QuadProps
    {
        vec3 position = vec3(0.0f);
        vec2 size = vec2(1.0f);
        vec4 color = vec4(1.0f);
        mat4 transform = mat4(1.0f);
        Ref<Texture2D> texture = nullptr;
        float rotation = 0.0f;

        float tilingFactor = 1.0f;
        uint32_t entityID = entt::null;
    };

    EditorRenderer2D(Ref<GraphicsContext> context) : context(context)
    {
        quadVertexArray = context->CreateVertexArray();
        Ref<VertexBuffer> quadVertexBuffer = context->CreateVertexBuffer(maxVertices * sizeof(QuadVertex));
        BufferLayout layout = {
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_TexCoord"},
            {ShaderDataType::Int, "a_TexIndex"},
            {ShaderDataType::UInt, "a_EntityID"}};
        quadVertexBuffer->SetLayout(layout);
        quadVertexArray->AddVertexBuffer(quadVertexBuffer);

        quadVertexBufferBase = new QuadVertex[maxVertices];

        uint32_t *quadIndices = new uint32_t[maxIndices];
        for (uint32_t i = 0, offset = 0; i < maxIndices; i += 6, offset += 4)
        {
            quadIndices[i] = offset;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;
            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset;
        }
        auto indexBuffer = context->CreateIndexBuffer(quadIndices, maxIndices);
        quadVertexArray->SetIndexBuffer(indexBuffer);
        delete[] quadIndices;

        TextureSpecification spec;
        spec.width = 1, spec.height = 1, spec.format = TextureFormat::RGBA8;
        auto whiteTextureData = 0xffffffff;
        Buffer buffer(&whiteTextureData, sizeof(whiteTextureData));
        whiteTexture = context->CreateTexture2D(spec, buffer);

        textureShader = context->CreateShader("assets/shaders/quad_shader.glsl");
        textureShader->Bind();

        int samplers[maxTextureSlots];
        for (int i = 0; i < maxTextureSlots; i++)
            samplers[i] = i;
        textureShader->Set("u_Textures", samplers, maxTextureSlots);

        textureSlots[0] = whiteTexture;
    }

    void BeginScene(const mat4 &viewProjectionMatrix)
    {
        textureShader->Bind();
        textureShader->Set("u_ViewProjection", viewProjectionMatrix);
        StartBatch();
    }

    void EndScene()
    {
        uint32_t dataSize = (uint8_t *)quadVertexBufferPtr - (uint8_t *)quadVertexBufferBase;
        Buffer buffer(quadVertexBufferBase, dataSize);
        auto vertexBuffer = quadVertexArray->GetVertexBuffers()[0];
        vertexBuffer->SetData(buffer);
        Flush();
    }

    void Flush()
    {
        for (int i = 0; i < textureSlotIndex; i++)
            textureSlots[i]->Bind(i);

        context->DrawIndexed(quadVertexArray, textureShader, quadIndexCount);
    }

    void StartBatch()
    {
        quadVertexBufferPtr = quadVertexBufferBase;
        quadIndexCount = 0;
        textureSlotIndex = 1;
    }
    void FlushAndReset()
    {
        EndScene();
        StartBatch();
    }

    void DrawQuad(const QuadProps &props)
    {
        if (quadIndexCount >= maxIndices)
            FlushAndReset();
        uint32_t textureIndex = 0;
        if (props.texture)
        {
            for (auto i = 1; i < textureSlotIndex; i++)
            {
                if (textureSlots[i]->GetRendererID() == props.texture->GetRendererID())
                {
                    textureIndex = i;
                    break;
                }
            }
            if (textureIndex == 0)
            {
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
            quadVertexBufferPtr->entityID = props.entityID;
            quadVertexBufferPtr++;
        }
        quadIndexCount += 6;
    }
  private:

    // 批处理绘制矩形
    struct QuadVertex
    {
        vec3 position;
        vec4 color;
        vec2 texCoord;
        int texIndex;
        uint32_t entityID;
    };

    Ref<GraphicsContext> context;
    Ref<VertexArray> quadVertexArray;
    Ref<Shader> textureShader;
    Ref<Texture2D> whiteTexture;
    static constexpr uint32_t maxQuads = 10000;
    static constexpr uint32_t maxVertices = maxQuads * 4;
    static constexpr uint32_t maxIndices = maxQuads * 6;
    static constexpr uint32_t maxTextureSlots = 32;
    Ref<Texture2D> textureSlots[maxTextureSlots];
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
}