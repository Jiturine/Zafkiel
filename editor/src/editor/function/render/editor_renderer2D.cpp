#include "editor_renderer2D.h"
#include "function/render/renderer.h"

namespace Zafkiel
{
#if 0
EditorRenderer2D::EditorRenderer2D()
{
    auto &context = Renderer::Instance().GetGraphicsContext();
    Ref<VertexBuffer> quadVertexBuffer = context->CreateVertexBuffer(maxVertices * sizeof(QuadVertex));
    VertexBufferLayout layout = {
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float2, "a_TexCoord"},
        {ShaderDataType::Int, "a_TexIndex"},
        {ShaderDataType::UInt, "a_EntityID"}};
    quadVertexBuffer->SetLayout(layout);

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

    Texture2DSpecification spec;
    spec.width = 1, spec.height = 1, spec.format = ImageFormat::RGBA8;
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

void EditorRenderer2D::BeginScene(const mat4 &viewProjectionMatrix)
{
    textureShader->Bind();
    textureShader->Set("u_ViewProjection", viewProjectionMatrix);
    StartBatch();
}

void EditorRenderer2D::EndScene()
{
    uint32_t dataSize = (uint8_t *)quadVertexBufferPtr - (uint8_t *)quadVertexBufferBase;
    Buffer buffer(quadVertexBufferBase, dataSize);
    quadVertexBuffer->SetData(buffer);
    Flush();
}

void EditorRenderer2D::Flush()
{
    for (int i = 0; i < textureSlotIndex; i++)
        textureSlots[i]->Bind(i);

    Renderer::Instance().GetGraphicsContext()->DrawIndexed(quadVertexBuffer, quadIndexBuffer, textureShader, quadIndexCount);
}

void EditorRenderer2D::StartBatch()
{
    quadVertexBufferPtr = quadVertexBufferBase;
    quadIndexCount = 0;
    textureSlotIndex = 1;
}
void EditorRenderer2D::FlushAndReset()
{
    EndScene();
    StartBatch();
}

void EditorRenderer2D::DrawQuad(const QuadProps &props)
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
#endif
}
