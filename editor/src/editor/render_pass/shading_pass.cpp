#include "shading_pass.h"
#include "function/render/renderer.h"
#include "function/render/render_command.h"

namespace Zafkiel
{

ShadingPass::ShadingPass(Observer<GlobalRenderResource> globalRenderResource, Observer<Texture2D> positionTexture, Observer<Texture2D> normalTexture, Observer<Texture2D> albedoTexture)
    : positionTexture(positionTexture), normalTexture(normalTexture), albedoTexture(albedoTexture)
{
    std::vector<AttachmentDescription> attachmentDescs 
    {
        {
            .format = ImageFormat::RGBA8,
            .initialLayout = ImageLayout::Undefined,
            .finalLayout = ImageLayout::ShaderReadOnly,
            .samples = 1
        },
    };
    RenderPassSpecification renderPassSpec 
    {
        .attachments = attachmentDescs,
        .subpasses = 
        {
            {
                .attachmentRefs = 
                {
                    {0, ImageLayout::ColorAttachment},
                },
                .type = PipelineType::Graphics
            }
        }
    };
    renderPass = Renderer::GetGraphicsContext()->CreateRenderPass(renderPassSpec);

    shader = Renderer::GetGraphicsContext()->CreateGraphicsShader("assets/shaders/deferred_shader.glsl");

    renderPassResource = Renderer::GetGraphicsContext()->CreateRenderPassResource("assets/shaders/schema/shading_pass.zss");
    renderPassResource->GetRenderResource()->SetTexture2D("Position", positionTexture);
    renderPassResource->GetRenderResource()->SetTexture2D("Normal", normalTexture);
    renderPassResource->GetRenderResource()->SetTexture2D("Albedo", albedoTexture);

    GraphicsPipelineSpecification pipelineSpec
    {
        .primitiveTopology = PrimitiveTopology::Triangles,
        .shader = shader,
        .renderResourceTemplates
        {
            globalRenderResource->GetRenderResource()->GetTemplate(),
            renderPassResource->GetRenderResource()->GetTemplate(),
        },
        .renderPass = renderPass,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .colorAttachmentCount = 1
    };
    pipeline = Renderer::GetGraphicsContext()->CreateGraphicsPipeline(pipelineSpec);

    // Textures
    Texture2DSpecification otuputColorTextureSpec
    {
        .width = 1280,
        .height = 720,
        .format = ImageFormat::RGBA8,
        .usages = { ImageUsage::Sampled, ImageUsage::ColorAttachment },
        .updateFrequency = ImageUpdateFrequency::Transient,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest
    };
    outputColorTexture = Renderer::GetGraphicsContext()->CreateTexture2D(otuputColorTextureSpec);

    FrameBufferSpecification frameBufferSpec
    {
        .width = 1280,
        .height = 720,
        .attachments = { outputColorTexture->GetImage() },
        .renderPass = renderPass,
    };
    frameBuffer = Renderer::GetGraphicsContext()->CreateFrameBuffer(frameBufferSpec);
    
    const float vertices[] =
    {
        -1.0f, -1.0f, 0.0f, 0.0f,  
        -1.0f,  1.0f, 0.0f, 1.0f,  
        1.0f,  1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f  
    };
    vertexBuffer = Renderer::GetGraphicsContext()->CreateVertexBuffer(vertices, sizeof(vertices));
    const uint32_t indices[] = 
    {
        0, 1, 2, 
        2, 3, 0
    };
    indexBuffer = Renderer::GetGraphicsContext()->CreateIndexBuffer(indices, 6);       
}


void ShadingPass::Render(const FrameData &frameData)
{
    RenderPassBeginInfo beginInfo
    {
        .renderPass = renderPass,
        .frameBuffer = frameBuffer,
        .clearValues = 
        {
            {.type = AttachmentType::Color, .format = ImageFormat::RGBA8, .vec4Value = vec4(0, 0, 0, 1) },
        }
    };
    RenderCommand::BeginRenderPass(beginInfo);
    
    RenderCommand::BindRenderPassResource(renderPassResource);
    
    RenderCommand::BindPipeline(pipeline);

    RenderCommand::DrawIndexed(vertexBuffer, indexBuffer);

    RenderCommand::EndRenderPass();      
}
void ShadingPass::Resize(uint32_t width, uint32_t height)
{
    outputColorTexture->Resize(width, height);
    frameBuffer->Resize(width, height);

    renderPassResource->GetRenderResource()->SetDirty("Position");
    renderPassResource->GetRenderResource()->SetDirty("Normal");
    renderPassResource->GetRenderResource()->SetDirty("Albedo");
}

}