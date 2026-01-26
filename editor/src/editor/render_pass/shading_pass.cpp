#include "editor/render_pass/shading_pass.h"
#include "function/render/backends/vulkan/vulkan_shader_material.h"
#include "function/render/renderer.h"
#include "function/render/render_command.h"

namespace Zafkiel
{

ShadingPass::ShadingPass(RenderHandle globalMaterial, RenderHandle positionTexture, RenderHandle normalTexture, RenderHandle albedoTexture)
    : globalMaterial(globalMaterial), positionTexture(positionTexture), normalTexture(normalTexture), albedoTexture(albedoTexture)
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
    renderPass = Renderer::Instance().CreateRenderPass(renderPassSpec);

    shader = Renderer::Instance().CreateGraphicsShader("assets/shaders/deferred_shader.glsl");

    passMaterial = Renderer::Instance().CreatePassMaterial("assets/shaders/schema/shading_pass.zss");
    Renderer::Instance().SetTexture2DFromPassMaterial(passMaterial, "Position", positionTexture);
    Renderer::Instance().SetTexture2DFromPassMaterial(passMaterial, "Normal", normalTexture);
    Renderer::Instance().SetTexture2DFromPassMaterial(passMaterial, "Albedo", albedoTexture);

    GraphicsPipelineSpecification pipelineSpec
    {
        .primitiveTopology = PrimitiveTopology::Triangles,
        .shader = shader,
        .shaderMaterialTemplates
        {
            Renderer::Instance().GetGlobalMaterial(globalMaterial)->GetTemplate(),
            Renderer::Instance().GetPassMaterial(passMaterial)->GetTemplate(),
        },
        .renderPass = renderPass,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .colorAttachmentCount = 1
    };
    pipeline = Renderer::Instance().CreateGraphicsPipeline(pipelineSpec);

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
    outputColorTexture = Renderer::Instance().CreateTexture2D(otuputColorTextureSpec);

    FrameBufferSpecification frameBufferSpec
    {
        .width = 1280,
        .height = 720,
        .attachments = { Renderer::Instance().GetTexture2D(outputColorTexture)->GetImage() },
        .renderPass = renderPass,
    };
    frameBuffer = Renderer::Instance().CreateFrameBuffer(frameBufferSpec);
    
    const float vertices[] =
    {
        -1.0f, -1.0f, 0.0f, 0.0f,  
        -1.0f,  1.0f, 0.0f, 1.0f,  
        1.0f,  1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f  
    };
    vertexBuffer = Renderer::Instance().CreateVertexBuffer(vertices, sizeof(vertices));
    const uint32_t indices[] = 
    {
        0, 1, 2, 
        2, 3, 0
    };
    indexBuffer = Renderer::Instance().CreateIndexBuffer(indices, 6);       
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
    Renderer::Instance().CmdBeginRenderPass(beginInfo);
    
    Renderer::Instance().CmdBindGlobalMaterial(globalMaterial);

    Renderer::Instance().CmdBindPassMaterial(passMaterial);

    Renderer::Instance().CmdBindGraphicsPipeline(pipeline);

    Renderer::Instance().CmdDrawIndexed(vertexBuffer, indexBuffer);

    Renderer::Instance().CmdEndRenderPass();
}
void ShadingPass::Resize(uint32_t width, uint32_t height)
{
    Renderer::Instance().ResizeTexture2D(outputColorTexture, width, height);
    Renderer::Instance().ResizeFrameBuffer(frameBuffer, width, height);

    auto passMaterialObj = Renderer::Instance().GetPassMaterial(passMaterial);
    auto shaderMaterial = Renderer::Instance().GetShaderMaterial(passMaterialObj->GetShaderMaterial());

    if (Renderer::Instance().GetGraphicsContext()->GetAPI() == GraphicsAPI::Vulkan)
    {
        auto shaderMaterialHandle = Renderer::Instance().GetPassMaterial(passMaterial)->GetShaderMaterial();
        auto shaderMaterial = Renderer::Instance().GetShaderMaterial(shaderMaterialHandle);
        auto backend = shaderMaterial->GetBackend().As<VulkanShaderMaterialBackend>();
        backend->SetDirty(0);
        backend->SetDirty(1);
        backend->SetDirty(2);
    }
}

}