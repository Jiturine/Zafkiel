#include "editor/RenderPass/PresentPass.h"
#include "Function/RHI/RHI.h"

namespace Zafkiel
{

PresentPass::PresentPass(Ref<RHIViewport> viewport)
    : viewport(viewport)
{
    vertexShader = GlobalRHI->CreateVertexShader("assets/shaders/PresentVS.glsl");

    fragmentShader = GlobalRHI->CreateFragmentShader("assets/shaders/PresentFS.glsl");

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

void PresentPass::Render(RHITexture *texture)
{
    // GlobalRHICmdList->RenderToScreen(viewport.get(), [&]() {

    //     pipeline->SetTexture(ShaderStage::Fragment, "uScreenTexture", texture);

    //     GlobalRHICmdList->BindGraphicsPipeline(pipeline.get());

    //     GlobalRHICmdList->DrawIndexed(vertexBuffer.get(), indexBuffer.get());
        
    // });
}

}