#include "editor/RenderPass/ShadingPass.h"
#include "editor/RenderPass/GeometryPass.h"
#include "editor/RenderPass/ShadowPass.h"
#include "Function/Render/Renderer.h"

namespace Zafkiel
{

ShadingPass::ShadingPass(GeometryPass *geometryPass, ShadowPass *shadowPass)
    : geometryPass(geometryPass), shadowPass(shadowPass)
{
    vertexShader = GlobalRHI->CreateVertexShader("assets/shaders/DeferredPBR_VS.glsl");
    fragmentShader = GlobalRHI->CreateFragmentShader("assets/shaders/DeferredPBR_FS.glsl");
    
    RHIGraphicsPipelineDesc pipelineDesc
    {
        .shaders = { vertexShader.get(), fragmentShader.get() },
        .primitiveTopology = PrimitiveTopology::Triangles,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .polygonMode = PolygonMode::Fill,
        .renderTargetDesc 
        {
            .colorAttachmentDescs
            {
                {
                    .format = ImageFormat::RGBA16F,
                    .sampleCount = 1,
                }
            }
        }
    };
    pipeline = GlobalRHI->CreateGraphicsPipeline(pipelineDesc);


    // Textures
    RHITextureDesc outputColorTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA16F,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::ColorAttachment,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest,
        .sampleCount = 1,
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


void ShadingPass::Render()
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

    pipeline->SetUniformBuffer(ShaderStage::Fragment, "uShadow", shadowPass->uShadowUniformBuffer.get());

    pipeline->SetTexture(ShaderStage::Fragment, "uPosition", geometryPass->positionTexture.get());
    pipeline->SetTexture(ShaderStage::Fragment, "uNormal", geometryPass->normalTexture.get());
    pipeline->SetTexture(ShaderStage::Fragment, "uAlbedo", geometryPass->albedoTexture.get());
    pipeline->SetTexture(ShaderStage::Fragment, "uMetalness", geometryPass->metalnessTexture.get());
    pipeline->SetTexture(ShaderStage::Fragment, "uRoughness", geometryPass->roughnessTexture.get());
    pipeline->SetTexture(ShaderStage::Fragment, "uShadowMap", shadowPass->shadowMap.get());

    GlobalRHICmdList->DrawIndexed(vertexBuffer.get(), indexBuffer.get());

    GlobalRHICmdList->EndRenderPass();
}

void ShadingPass::Resize(uint32 width, uint32 height)
{
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(outputColorTexture, width, height);
#if 0
    Renderer::Instance().ResizeTexture2D(outputColorTexture, width, height);
    Renderer::Instance().ResizeFrameBuffer(frameBuffer, width, height);

    auto pipelineMaterialObj = Renderer::Instance().GetPipelineMaterial(pipelineMaterial);
    auto shaderMaterial = Renderer::Instance().GetShaderMaterial(pipelineMaterialObj->GetShaderMaterial());

    if (Renderer::Instance().GetGraphicsContext()->GetAPI() == GraphicsAPI::Vulkan)
    {
        auto shaderMaterialHandle = Renderer::Instance().GetPipelineMaterial(pipelineMaterial)->GetShaderMaterial();
        auto shaderMaterial = Renderer::Instance().GetShaderMaterial(shaderMaterialHandle);
        auto backend = shaderMaterial->GetBackend().As<VulkanShaderMaterialBackend>();
        backend->SetDirty(0);
        backend->SetDirty(1);
        backend->SetDirty(2);
        backend->SetDirty(3);
        backend->SetDirty(4);
    }
#endif
}

}
