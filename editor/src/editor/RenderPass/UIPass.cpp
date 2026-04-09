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
    quadVertexBuffer = GlobalRHICmdList->CreateBuffer(quadVertexBufferDesc);
    
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
                    .sampleCount = 1,
                }
            }
        }
    };
    pipeline = GlobalRHI->CreateGraphicsPipeline(pipelineDesc);

    RHIBufferDesc quadUniformBufferDesc 
    {
        .size = maxIndices * sizeof(uint32),
        .usages = BufferUsageFlags::UniformBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Dynamic,
    };
    quadUniformBuffer = GlobalRHICmdList->CreateBuffer(quadUniformBufferDesc);
    
    pipeline->SetUniformBuffer(ShaderStage::Vertex, "uQuad", quadUniformBuffer.get());

    quadUniformBufferContent = CreateRef<UniformBufferContent>(quadVertexShader->GetResourceTable().GetResourceType("UIQuadUBO")->As<ShaderReflection::UniformBlock>());

    textureSlots[0] = whiteTexture;
}

QuadPipeline::~QuadPipeline()
{
    delete[] quadVertexBufferBase;
    quadVertexBufferBase = nullptr;
    quadVertexBufferPtr = nullptr;
}

void QuadPipeline::Render(uint32 width, uint32 height)
{
    mat4 viewMatrix = Maths::LookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 orthoProjMatrix = Maths::Ortho(0, width, 0, height, 0.1f, 100.0f);
    mat4 viewProj = orthoProjMatrix * viewMatrix;
    BeginScene(viewProj);
    QuadProps quadProps 
    {
        .position = vec3(200, 200, 0.0), // 中心点
        .size = vec2(100, 100),
        .texture = whiteTexture,
    };
    DrawQuad(quadProps);
    quadProps.position.x += 400;
    DrawQuad(quadProps);
    EndScene();
}

void QuadPipeline::BeginScene(const mat4 &viewProjectionMatrix)
{
    quadUniformBufferContent->SetParameter("viewProjection", viewProjectionMatrix, ShaderFundamentalType::Mat4);

    GlobalRHICmdList->UpdateUniformBuffer(quadUniformBuffer.get(), quadUniformBufferContent->GetData());

    for (auto index = 0; index < maxTextureSlots; index++)
    {
        textureSlots[index] = whiteTexture;
    }

    StartBatch();
}

void QuadPipeline::EndScene()
{
    if (quadIndexCount == 0)
        return;

    Flush();
}

void QuadPipeline::Flush()
{
    if (quadIndexCount == 0)
        return;

    uint32 size = (quadVertexBufferPtr - quadVertexBufferBase) * sizeof(QuadVertex);

    GlobalRHICmdList->WriteBuffer(quadVertexBuffer.get(), Buffer(reinterpret_cast<uint8 *>(quadVertexBufferBase), size));

    for (uint32 index = 0; index < maxTextureSlots; index++)
    {
        pipeline->SetTexture(ShaderStage::Fragment, std::format("uTexture{}", index), textureSlots[index].get());
    }

    GlobalRHICmdList->BindGraphicsPipeline(pipeline.get());

    GlobalRHICmdList->DrawIndexed(quadVertexBuffer.get(), quadIndexBuffer.get(), quadIndexCount);
}

void QuadPipeline::StartBatch()
{
    quadVertexBufferPtr = quadVertexBufferBase;
    quadIndexCount = 0;
    textureSlotIndex = 1;
}

void QuadPipeline::FlushAndReset()
{
    EndScene();
    StartBatch();
}

void QuadPipeline::DrawQuad(const QuadProps &props)
{
    if (quadIndexCount >= maxIndices)
        FlushAndReset();

    uint32 textureIndex = -1;
    if (props.texture)
    {
        for (uint32 i = 0; i < textureSlotIndex; ++i)
        {
            if (textureSlots[i] == props.texture)
            {
                textureIndex = i;
                break;
            }
        }

        if (textureIndex == -1)
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

void QuadPipeline::Resize(uint32 width, uint32 height)
{
    // if (Renderer::Instance().GetGraphicsContext()->GetAPI() == GraphicsAPI::Vulkan)
    // {
    //     auto shaderMaterialHandle = Renderer::Instance().GetPipelineMaterial(pipelineMaterial)->GetShaderMaterial();
    //     auto shaderMaterial = Renderer::Instance().GetShaderMaterial(shaderMaterialHandle);
    //     auto backend = shaderMaterial->GetBackend().As<VulkanShaderMaterialBackend>();
    //     backend->SetDirty(0);
    // }
}

UIPass::UIPass(PostProcessingPass *postProcessingPass)
{
    RHITextureDesc outputColorTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA8,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::ColorAttachment,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    outputTexture = GlobalRHICmdList->CreateTexture(outputColorTextureDesc);
    
    uiCompositePipeline = CreateScope<UICompositePipeline>(postProcessingPass);

    quadPipeline = CreateScope<QuadPipeline>();
}

void UIPass::Render()
{
    RHIRenderPassInfo beginInfo
    {
        .colorAttachments
        {
            {
                .texture = outputTexture.get(),
                .clearValue = { .vec4Value = vec4(0, 0, 0, 1) },
                .initialLayout = ImageLayout::Undefined,
                .finalLayout = ImageLayout::ShaderReadOnly
            }
        }
    };
    GlobalRHICmdList->BeginRenderPass(beginInfo);

    uiCompositePipeline->Render();
    
    quadPipeline->Render(currentWidth, currentHeight);

    GlobalRHICmdList->EndRenderPass();
}

void UIPass::Resize(uint32 width, uint32 height)
{
    currentWidth = width;
    currentHeight = height;

    Renderer::Instance().GetRenderTargetPool().UpdateTexture(outputTexture, width, height);

    // uiCompositePipeline->Resize();

    quadPipeline->Resize(width, height);
}


UICompositePipeline::UICompositePipeline(PostProcessingPass *postProcessingPass)
    : postProcessingPass(postProcessingPass)
{
    const float vertices[] =
    {
        -1.0f, -1.0f, 0.0f, 0.0f,  
        -1.0f,  1.0f, 0.0f, 1.0f,  
        1.0f,  1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f  
    };
    RHIBufferDesc vertexBufferDesc 
    {
        .size = sizeof(vertices),
        .usages = BufferUsageFlags::VertexBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Static,
    };
    vertexBuffer = GlobalRHICmdList->CreateBuffer(vertexBufferDesc, vertices);

    const uint32 indices[] = 
    {
        0, 1, 2, 
        2, 3, 0
    };
    RHIBufferDesc indexBufferDesc
    {
        .size = sizeof(indices),
        .usages = BufferUsageFlags::IndexBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Static,
    };
    indexBuffer = GlobalRHICmdList->CreateBuffer(indexBufferDesc, indices);

    vertexShader = GlobalRHI->CreateVertexShader("assets/shaders/UICompositeVS.glsl");

    fragmentShader = GlobalRHI->CreateFragmentShader("assets/shaders/UICompositeFS.glsl");

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
                    .sampleCount = 1,
                }
            }
        }
    };
    pipeline = GlobalRHI->CreateGraphicsPipeline(pipelineDesc);
}

void UICompositePipeline::Render()
{
    GlobalRHICmdList->BindGraphicsPipeline(pipeline.get());

    pipeline->SetTexture(ShaderStage::Fragment, "uScreenTexture", postProcessingPass->outputColorTexture.get());

    GlobalRHICmdList->DrawIndexed(vertexBuffer.get(), indexBuffer.get());
}

void UICompositePipeline::Resize()
{
    // if (Renderer::Instance().GetGraphicsContext()->GetAPI() == GraphicsAPI::Vulkan)
    // {
    //     auto shaderMaterialHandle = Renderer::Instance().GetPipelineMaterial(pipelineMaterial)->GetShaderMaterial();
    //     auto shaderMaterial = Renderer::Instance().GetShaderMaterial(shaderMaterialHandle);
    //     auto backend = shaderMaterial->GetBackend().As<VulkanShaderMaterialBackend>();
    //     backend->SetDirty(0);
    // }
}

#if 0

TextPipeline::TextPipeline(RenderHandle uiPass)
{
    textVertexBufferBase = new TextVertex[maxVertices];

    Texture2DSpecification spec
    {
        .width = 1,
        .height = 1,
        .format = ImageFormat::RGBA8,
        .usages = { ImageUsage::Sampled, ImageUsage::Upload },
        .updateFrequency = ImageUpdateFrequency::Static
    };

    textShader = Renderer::Instance().CreateGraphicsShader("assets/shaders/text_shader.glsl");
    pipelineMaterial = Renderer::Instance().CreatePipelineMaterial("assets/shaders/schema/ui_text_pipeline.zss");
    auto shaderMaterial = Renderer::Instance().GetPipelineMaterial(pipelineMaterial)->GetShaderMaterial();

    GraphicsPipelineSpecification pipelineSpec
    {
        .primitiveTopology = PrimitiveTopology::Triangles,
        .shader = textShader,
        .shaderMaterialTemplates
        {
            {},
            Renderer::Instance().GetPipelineMaterial(pipelineMaterial)->GetTemplate(),
            {},
            {}
        },
        .renderPass = uiPass,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .depthTest = false,
        .colorAttachmentCount = 1
    };
    pipeline = Renderer::Instance().CreateGraphicsPipeline(pipelineSpec);
}

TextPipeline::~TextPipeline()
{
    delete[] textVertexBufferBase;
    textVertexBufferBase = nullptr;
    textVertexBufferPtr = nullptr;
}
#endif

}
