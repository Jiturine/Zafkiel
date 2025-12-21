#include "geometry_pass.h"
#include "function/render/renderer.h"
#include "function/render/render_command.h"
#include "function/scene/components.h"
#include "function/scene/scene_manager.h"

namespace Zafkiel
{

GeometryPass::GeometryPass(Observer<GlobalRenderResource> globalRenderResource, Observer<ObjectRenderResource> objectRenderResource)
    : objectRenderResource(objectRenderResource)
{
    std::vector<AttachmentDescription> attachmentDescs
    {
        { // gPosition
            .format = ImageFormat::RGBA16F,
            .initialLayout = ImageLayout::Undefined,
            .finalLayout = ImageLayout::ShaderReadOnly,
            .samples = 1
        },
        { // gNormal
            .format = ImageFormat::RGBA16F,
            .initialLayout = ImageLayout::Undefined,
            .finalLayout = ImageLayout::ShaderReadOnly,
            .samples = 1
        },
        { // gAlbedo
            .format = ImageFormat::RGBA8,
            .initialLayout = ImageLayout::Undefined,
            .finalLayout = ImageLayout::ShaderReadOnly,
            .samples = 1
        },
        { //gEntityID
            .format = ImageFormat::R32UI,
            .initialLayout = ImageLayout::Undefined,
            .finalLayout = ImageLayout::ShaderReadOnly,
            .samples = 1
        },
        { // Depth Attachment
            .format = ImageFormat::DEPTH24STENCIL8,
            .initialLayout = ImageLayout::Undefined,
            .finalLayout = ImageLayout::DepthStencilAttachment,
            .samples = 1
        }
    };
    std::vector<SubpassSpecification> subpassSpecs
    {
        {
            .attachmentRefs = {
                {0, ImageLayout::ColorAttachment},
                {1, ImageLayout::ColorAttachment},
                {2, ImageLayout::ColorAttachment},
                {3, ImageLayout::ColorAttachment},
                {4, ImageLayout::DepthStencilAttachment}
            },
            .type = PipelineType::Graphics
        }
    };
    RenderPassSpecification renderPassSpec
    {
        .attachments = attachmentDescs,
        .subpasses = subpassSpecs
    };
    renderPass = Renderer::GetGraphicsContext()->CreateRenderPass(renderPassSpec);
    
    blinnPhongShader = Renderer::GetGraphicsContext()->CreateGraphicsShader("assets/shaders/gbuffer_shader.glsl");

    GraphicsPipelineSpecification pipelineSpec
    {
        .primitiveTopology = PrimitiveTopology::Triangles,
        .shader = blinnPhongShader,
        .renderResourceTemplates 
        {
            globalRenderResource->GetRenderResource()->GetTemplate(),
            {},
            Renderer::GetBuiltInMaterialTemplate(ShaderFamily::BlinnPhong),
            objectRenderResource->GetTemplate()
        },
        .renderPass = renderPass,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .depthTest = true,
        .colorAttachmentCount = 4,
    };
    blinnPhongPipeline = Renderer::GetGraphicsContext()->CreateGraphicsPipeline(pipelineSpec);

    // Textures
    Texture2DSpecification positionTextureSpec
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA16F,
        .usages = { ImageUsage::Sampled, ImageUsage::ColorAttachment },
        .updateFrequency = ImageUpdateFrequency::Transient,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    positionTexture = Renderer::GetGraphicsContext()->CreateTexture2D(positionTextureSpec);
    Texture2DSpecification normalTextureSpec
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA16F,
        .usages = { ImageUsage::Sampled, ImageUsage::ColorAttachment },
        .updateFrequency = ImageUpdateFrequency::Transient,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    normalTexture = Renderer::GetGraphicsContext()->CreateTexture2D(normalTextureSpec); 
    Texture2DSpecification albedoTextureSpec
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA8,
        .usages = { ImageUsage::Sampled, ImageUsage::ColorAttachment },
        .updateFrequency = ImageUpdateFrequency::Transient,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    albedoTexture = Renderer::GetGraphicsContext()->CreateTexture2D(albedoTextureSpec); 
    Texture2DSpecification entityIDTextureSpec
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::R32UI,
        .usages = { ImageUsage::Sampled, ImageUsage::ColorAttachment },
        .updateFrequency = ImageUpdateFrequency::Transient,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    entityIDTexture = Renderer::GetGraphicsContext()->CreateTexture2D(entityIDTextureSpec);
    
    ImageSpecification depthImageSpec
    {
        .format = ImageFormat::DEPTH24STENCIL8,
        .usages = { ImageUsage::DepthAttachment },
        .updateFrequency = ImageUpdateFrequency::Dynamic,
        .width = 1280,
        .height = 720,
        .samples = 1,
    };
    depthImage = Renderer::GetGraphicsContext()->CreateImage(depthImageSpec);
    
    FrameBufferSpecification frameBufferSpec
    {
        .width = 1280,
        .height = 720,
        .attachments = 
        {
            positionTexture->GetImage(),
            normalTexture->GetImage(),
            albedoTexture->GetImage(),
            entityIDTexture->GetImage(),
            depthImage
        },
        .renderPass = renderPass,
    };
    frameBuffer = Renderer::GetGraphicsContext()->CreateFrameBuffer(frameBufferSpec);
}

void GeometryPass::Render(const FrameData &frameData)
{
    RenderPassBeginInfo beginInfo
    {
        .renderPass = renderPass,
        .frameBuffer = frameBuffer,
        .clearValues = 
        {
           {.type = AttachmentType::Color, .format = ImageFormat::RGBA16F, .vec4Value = vec4(0, 0, 0, 1) },
           {.type = AttachmentType::Color, .format = ImageFormat::RGBA16F, .vec4Value = vec4(0, 0, 0, 1) },
           {.type = AttachmentType::Color, .format = ImageFormat::RGBA8, .vec4Value = vec4(0, 0, 0, 1) },
           {.type = AttachmentType::Color, .format = ImageFormat::R32UI, .uintValue = (uint32_t)entt::null },
           {.type = AttachmentType::Depth, .format = ImageFormat::DEPTH24STENCIL8, .floatValue = 1.0f }
        }
    };
    RenderCommand::BeginRenderPass(beginInfo);
    
    RenderCommand::BindPipeline(blinnPhongPipeline);
    
    for (auto renderable : frameData.renderables)
    {
        Ref<MeshAsset> meshAsset = AssetManager::GetAsset(renderable.mesh).As<MeshAsset>();
        Ref<MaterialAsset> materialAsset = AssetManager::GetAsset(renderable.material).As<MaterialAsset>();
        
        RenderCommand::BindMaterial(Renderer::GetMaterial(materialAsset));
        
        RenderCommand::BindObjectRenderResource(renderable.index, objectRenderResource);

        auto mesh = Renderer::GetMesh(meshAsset);
        RenderCommand::DrawIndexed(mesh->GetVertexBuffer(), mesh->GetIndexBuffer());
    }

    RenderCommand::EndRenderPass();
}

void GeometryPass::Resize(uint32_t width, uint32_t height)
{
    positionTexture->Resize(width, height);
    normalTexture->Resize(width, height);
    albedoTexture->Resize(width, height);
    entityIDTexture->Resize(width, height);
    depthImage->Resize(width, height);
    frameBuffer->Resize(width, height);
}

}