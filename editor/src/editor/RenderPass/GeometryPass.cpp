#include "editor/RenderPass/GeometryPass.h"
#include "Function/Render/Renderer.h"
#include "Function/Scene/Components.h"
#include "Resource/AssetManager.h"
#include "Resource/MaterialAsset.h"

namespace Zafkiel
{

GeometryPass::GeometryPass()
{
    vertexShader = GlobalRHI->CreateVertexShader("assets/shaders/GBufferVS.glsl");

    fragmentShader = GlobalRHI->CreateFragmentShader("assets/shaders/GBufferFS.glsl");

    RHIGraphicsPipelineDesc pipelineDesc
    {
        .shaders = { vertexShader.get(), fragmentShader.get() },
        .primitiveTopology = PrimitiveTopology::Triangles,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .depthTest = false, // TODO: debug
        .renderTargetDesc
        {
            .colorAttachmentDescs
            {
                { // gPosition
                    .format = ImageFormat::RGBA16F,
                    .sampleCount = 1
                },
                { // gNormal
                    .format = ImageFormat::RGBA16F,
                    .sampleCount = 1
                },
                { // gAlbedo
                    .format = ImageFormat::RGBA8,
                    .sampleCount = 1
                },
                { // gMetalness
                    .format = ImageFormat::R8,
                    .sampleCount = 1
                },
                { // gRoughness
                    .format = ImageFormat::R8,
                    .sampleCount = 1
                },
                { //gEntityID
                    .format = ImageFormat::R32UI,
                    .sampleCount = 1
                },
            },
            .depthStencilAttachmentDesc = RenderTargetDesc::DepthStencilAttachmentDesc 
            { 
                .format = ImageFormat::DEPTH32F,
                .sampleCount = 1,
            }
        }
    };

    pbrPipeline = GlobalRHI->CreateGraphicsPipeline(pipelineDesc);

    // Textures
    RHITextureDesc positionTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA16F,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::ColorAttachment,
        .initialLayout = ImageLayout::ShaderReadOnly,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    positionTexture = GlobalRHICmdList->CreateTexture(positionTextureDesc);
    RHITextureDesc normalTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA16F,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::ColorAttachment,
        .initialLayout = ImageLayout::ShaderReadOnly,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    normalTexture = GlobalRHICmdList->CreateTexture(normalTextureDesc); 
    RHITextureDesc albedoTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA8,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::ColorAttachment,
        .initialLayout = ImageLayout::ShaderReadOnly,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    albedoTexture = GlobalRHICmdList->CreateTexture(albedoTextureDesc); 
    RHITextureDesc metalnessTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::R8,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::ColorAttachment,
        .initialLayout = ImageLayout::ShaderReadOnly,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    metalnessTexture = GlobalRHICmdList->CreateTexture(metalnessTextureDesc); 
    RHITextureDesc roughnessTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::R8,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::ColorAttachment,
        .initialLayout = ImageLayout::ShaderReadOnly,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    roughnessTexture = GlobalRHICmdList->CreateTexture(roughnessTextureDesc); 
    RHITextureDesc entityIDTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::R32UI,
        .usages = ImageUsageFlags::Sampled | ImageUsageFlags::ColorAttachment,
        .initialLayout = ImageLayout::ShaderReadOnly,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    entityIDTexture = GlobalRHICmdList->CreateTexture(entityIDTextureDesc);
    
    RHITextureDesc depthTextureDesc
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::DEPTH32F,
        .usages = ImageUsageFlags::DepthAttachment,
        .initialLayout = ImageLayout::DepthStencilAttachment,
        .sampleCount = 1,
    };
    depthTexture = GlobalRHICmdList->CreateTexture(depthTextureDesc);
}

void GeometryPass::Render(const FrameData &frameData)
{
    RHIRenderPassInfo renderPassInfo
    {
        .colorAttachments
        {
            {
                .texture = positionTexture.get(),
                .clearValue = { .vec4Value = vec4(0, 0, 0, 1) },
                .initialLayout = ImageLayout::ShaderReadOnly,
                .finalLayout = ImageLayout::ShaderReadOnly,
            },
            {
                .texture = normalTexture.get(),
                .clearValue = { .vec4Value = vec4(0, 0, 0, 1) },
                .initialLayout = ImageLayout::ShaderReadOnly,
                .finalLayout = ImageLayout::ShaderReadOnly,
            },
            {
                .texture = albedoTexture.get(),
                .clearValue = { .vec4Value = vec4(0, 0, 0, 1) },
                .initialLayout = ImageLayout::ShaderReadOnly,
                .finalLayout = ImageLayout::ShaderReadOnly,
            },
            {
                .texture = metalnessTexture.get(),
                .clearValue = { .floatValue = 0 },
                .initialLayout = ImageLayout::ShaderReadOnly,
                .finalLayout = ImageLayout::ShaderReadOnly,
            },
            { 
                .texture = roughnessTexture.get(),
                .clearValue = { .floatValue = 0 },
                .initialLayout = ImageLayout::ShaderReadOnly,
                .finalLayout = ImageLayout::ShaderReadOnly,
            },
            {
                .texture = entityIDTexture.get(),
                .clearValue = { .uintValue = (uint32)entt::null },
                .initialLayout = ImageLayout::ShaderReadOnly,
                .finalLayout = ImageLayout::ShaderReadOnly,
            },
        },
        .depthStencilAttachment = RHIRenderPassInfo::DepthStencilAttachmentInfo
        {
            .texture = depthTexture.get(),
            .clearValue = { .floatValue = 1.0f },
            .initialLayout = ImageLayout::DepthStencilAttachment,
            .finalLayout = ImageLayout::DepthStencilAttachment,
        }
    };

    GlobalRHICmdList->BeginRenderPass(renderPassInfo);

    GlobalRHICmdList->BindGraphicsPipeline(pbrPipeline.get());

    MaterialDesc pbrMaterialDesc 
    {
        .textureMap
        {
            {"DiffuseTexture", "uDiffuseTexture"},
            {"NormalTexture", "uNormalTexture"},
            {"MetalnessTexture", "uMetalnessTexture"},
            {"RoughnessTexture", "uRoughnessTexture"},
        }
    };
    for (auto renderable : frameData.renderables)
    {
        pbrPipeline->SetDynamicOffsetIndex(ShaderStage::Vertex, "uMeshObject", renderable.index);

        auto mesh = Renderer::Instance().GetOrCreateMesh(renderable.meshAssetHandle);

        auto material = Renderer::Instance().GetOrCreateMaterial(renderable.materialAssetHandle, pbrMaterialDesc);

        auto* diffuseTex = material->GetTexture("uDiffuseTexture");
        auto* normalTex = material->GetTexture("uNormalTexture");
        auto* metalnessTex = material->GetTexture("uMetalnessTexture");
        auto* roughnessTex = material->GetTexture("uRoughnessTexture");

        if (diffuseTex) pbrPipeline->SetTexture(ShaderStage::Fragment, "uDiffuseTexture", diffuseTex);
        if (normalTex) pbrPipeline->SetTexture(ShaderStage::Fragment, "uNormalTexture", normalTex);
        if (metalnessTex) pbrPipeline->SetTexture(ShaderStage::Fragment, "uMetalnessTexture", metalnessTex);
        if (roughnessTex) pbrPipeline->SetTexture(ShaderStage::Fragment, "uRoughnessTexture", roughnessTex);

        GlobalRHICmdList->DrawIndexed(mesh->GetVertexBuffer().get(), mesh->GetIndexBuffer().get());
    }

    GlobalRHICmdList->EndRenderPass();
}

void GeometryPass::Resize(uint32 width, uint32 height)
{
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(positionTexture, width, height);
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(normalTexture, width, height);
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(albedoTexture, width, height);
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(metalnessTexture, width, height);
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(roughnessTexture, width, height);
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(entityIDTexture, width, height);
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(depthTexture, width, height);
}

}