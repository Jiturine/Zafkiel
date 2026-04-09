#include "editor/RenderPass/PostProcessingPass.h"
#include "editor/RenderPass/ShadingPass.h"
#include "Function/Render/Renderer.h"

namespace Zafkiel 
{


PostProcessingPass::PostProcessingPass(ShadingPass *shadingPass)
    : shadingPass(shadingPass)
{
    vertexShader = GlobalRHI->CreateVertexShader("assets/shaders/PostProcessingVS.glsl");

    fragmentShader = GlobalRHI->CreateFragmentShader("assets/shaders/PostProcessingFS.glsl");

    RHIGraphicsPipelineDesc pipelineDesc
    {
        .shaders = { vertexShader.get(), fragmentShader.get() },
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

    RHITextureDesc outputColorTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA8,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::ColorAttachment,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest,
    };
    outputColorTexture = GlobalRHICmdList->CreateTexture(outputColorTextureDesc);
    
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
}
void PostProcessingPass::Render()
{
    RHIRenderPassInfo renderPassInfo
    {
        .colorAttachments
        {
            {
                .texture = outputColorTexture.get(),
                .clearValue = { .vec4Value = vec4(0, 0, 0, 1) },
                .initialLayout = ImageLayout::Undefined,
                .finalLayout = ImageLayout::ShaderReadOnly,
            },
        },
    };
    GlobalRHICmdList->BeginRenderPass(renderPassInfo);
    
    GlobalRHICmdList->BindGraphicsPipeline(pipeline.get());

    pipeline->SetTexture(ShaderStage::Fragment, "uScreenTexture", shadingPass->outputColorTexture.get());

    GlobalRHICmdList->DrawIndexed(vertexBuffer.get(), indexBuffer.get());

    GlobalRHICmdList->EndRenderPass();
}

void PostProcessingPass::Resize(uint32 width, uint32 height)
{
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(outputColorTexture, width, height);
}
  
}
