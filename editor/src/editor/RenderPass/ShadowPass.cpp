#include "editor/RenderPass/ShadowPass.h"
#include "Function/Render/Renderer.h"
#include "Function/RHI/RHI.h"
#include "Function/RHI/RHICommandList.h"
#include "Resource/AssetManager.h"
#include "Resource/MeshAsset.h"

namespace Zafkiel 
{
  constexpr uint32 shadowMapSize = 2048;

ShadowPass::ShadowPass()
{
    vertexShader = GlobalRHI->CreateVertexShader("assets/shaders/ShadowVS.glsl");
    
    fragmentShader = GlobalRHI->CreateFragmentShader("assets/shaders/ShadowFS.glsl");

    RHIGraphicsPipelineDesc pipelineDesc
    {
        .shaders = { vertexShader.get(), fragmentShader.get() },
        .primitiveTopology = PrimitiveTopology::Triangles,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .polygonMode = PolygonMode::Fill,
        .depthTest = true,
        .renderTargetDesc 
        {
            .depthStencilAttachmentDesc = RenderTargetDesc::DepthStencilAttachmentDesc 
            { 
                .format = ImageFormat::DEPTH32F,
                .sampleCount = 1,
            }
        }
    };
    pipeline = GlobalRHI->CreateGraphicsPipeline(pipelineDesc);
    
    RHITextureDesc shadowMapDesc
    {
        .width = shadowMapSize,
        .height = shadowMapSize,
        .format = ImageFormat::DEPTH32F,
        .usages = ImageUsageFlags::DepthAttachment | ImageUsageFlags::Sampled,
        .initialLayout = ImageLayout::ShaderReadOnly,
        .filter = TextureFilter::Nearest,
        .sampleCount = 1,
    };
    shadowMap = GlobalRHICmdList->CreateTexture(shadowMapDesc);

    auto &uniformBufferType = vertexShader->GetResourceTable().customResourceTypes["ShadowUBO"].As<ShaderReflection::UniformBlock>();
    uShadowContent = CreateScope<UniformBufferContent>(&uniformBufferType);

    RHIBufferDesc uniformBufferDesc
    {
        .size = uniformBufferType.GetSize(),
        .usages = BufferUsageFlags::UniformBuffer | BufferUsageFlags::Dynamic | BufferUsageFlags::CPUAccessible,
    };
    uShadowUniformBuffer = GlobalRHICmdList->CreateBuffer(uniformBufferDesc);

    pipeline->SetUniformBuffer(ShaderStage::Vertex, "uShadow", uShadowUniformBuffer.get());

}

void ShadowPass::Render(const FrameData &frameData)
{
    RHIRenderPassInfo beginInfo
    {
        .depthStencilAttachment = RHIRenderPassInfo::DepthStencilAttachmentInfo
        {
            .texture = shadowMap.get(),
            .clearValue = {.floatValue = 1.0f },
            .initialLayout = ImageLayout::ShaderReadOnly,
            .finalLayout = ImageLayout::ShaderReadOnly,
        }
    };
    GlobalRHICmdList->BeginRenderPass(beginInfo);
    
    GlobalRHICmdList->BindGraphicsPipeline(pipeline.get());

    float nearZ, farZ;
    if (Maths::GetAPI() == GraphicsAPI::Vulkan)
    {
        nearZ = 0.0f;
        farZ = 1.0f;
    }
    else  // OpenGL
    {
        nearZ = -1.0f;
        farZ = 1.0f;
    }

    vec3 frustumCorners[8] =
    {
        {-1, -1, nearZ},
        { 1, -1, nearZ},
        { 1,  1, nearZ},
        {-1,  1, nearZ},

        {-1, -1, farZ},
        { 1, -1, farZ},
        { 1,  1, farZ},
        {-1,  1, farZ},
    };
    mat4 invViewProj = inverse(frameData.projectionMatrix * frameData.viewMatrix);

    for (int i = 0; i < 8; i++) 
    {
        vec4 invCorner = invViewProj * vec4(frustumCorners[i], 1.0f);
        frustumCorners[i] = invCorner / invCorner.w;
    }

    vec3 frustumCenter = vec3(0.0f);
    for (uint32 i = 0; i < 8; i++)
        frustumCenter += frustumCorners[i];
    frustumCenter /= 8.0f;

    // 计算包围球
    float radius = 0.0f;
    for (uint32 i = 0; i < 8; i++)
    {
        float distance = glm::length(frustumCorners[i] - frustumCenter);
        radius = glm::max(radius, distance);
    }

    vec3 eye = frustumCenter - glm::normalize(frameData.directionalLight.direction) * radius;

    vec3 up = glm::dot(frameData.directionalLight.direction, glm::vec3(0.0f, 1.0f, 0.0f)) > 0.99f ? vec3(1, 0, 0) : vec3(0, 1, 0);
    
    mat4 lightView = Maths::LookAt(eye, frustumCenter, up);
    mat4 lightProj = Maths::Ortho(-radius, radius, -radius, radius, 0, 2 * radius);
    lightViewProj = lightProj * lightView;

    uShadowContent->SetParameter("lightViewProjection", lightViewProj, ShaderFundamentalType::Mat4);

    for (auto renderable : frameData.renderables)
    {
        pipeline->SetDynamicOffsetIndex(ShaderStage::Vertex, "uMeshObject", renderable.index);
    
        auto mesh = Renderer::Instance().GetOrCreateMesh(renderable.meshAssetHandle);

        GlobalRHICmdList->DrawIndexed(mesh->GetVertexBuffer().get(), mesh->GetIndexBuffer().get());
    }

    GlobalRHICmdList->EndRenderPass();
}

}
