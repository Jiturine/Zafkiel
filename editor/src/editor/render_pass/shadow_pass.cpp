#include "editor/render_pass/shadow_pass.h"
#include "function/render/renderer.h"
#include "function/render/render_command.h"
#include "resource/asset_manager.h"
#include "resource/mesh_asset.h"

namespace Zafkiel 
{
  constexpr uint32_t shadowMapSize = 2048;

ShadowPass::ShadowPass(RenderHandle objectShaderMaterial)
    : objectShaderMaterial(objectShaderMaterial)
{
    std::vector<AttachmentDescription> attachmentDescs 
    {
        {
            .format = ImageFormat::DEPTH32F,
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
                    {0, ImageLayout::DepthAttachment},
                },
                .type = PipelineType::Graphics
            }
        }
    };
    renderPass = Renderer::Instance().CreateRenderPass(renderPassSpec);
    
    shader = Renderer::Instance().CreateGraphicsShader("assets/shaders/shadow_shader.glsl");

    passMaterial = Renderer::Instance().CreatePassMaterial("assets/shaders/schema/shadow_pass.zss");

    GraphicsPipelineSpecification pipelineSpec
    {
        .primitiveTopology = PrimitiveTopology::Triangles,
        .shader = shader,
        .shaderMaterialTemplates
        {
            {},
            Renderer::Instance().GetPassMaterial(passMaterial)->GetTemplate(),
            {},
            Renderer::Instance().GetObjectShaderMaterial(objectShaderMaterial)->GetTemplate(),
        },
        .renderPass = renderPass,
        .cullMode = CullMode::None,
        .frontFace = FrontFace::CounterClockWise,
        .depthTest = true,
        .colorAttachmentCount = 0
    };
    pipeline = Renderer::Instance().CreateGraphicsPipeline(pipelineSpec);
    
    Texture2DSpecification shadowMapSpec
    {
        .width = shadowMapSize,
        .height = shadowMapSize,
        .format = ImageFormat::DEPTH32F,
        .usages = {ImageUsage::DepthAttachment, ImageUsage::Sampled},
        .updateFrequency = ImageUpdateFrequency::Transient,
        .filter = TextureFilter::Nearest,
        .samples = 1,
    };
    shadowMap = Renderer::Instance().CreateTexture2D(shadowMapSpec);
    
    FrameBufferSpecification frameBufferSpec
    {
        .width = shadowMapSize,
        .height = shadowMapSize,
        .attachments
        {
            Renderer::Instance().GetTexture2D(shadowMap)->GetImage()
        },
        .renderPass = renderPass,
    };
    shadowFrameBuffer = Renderer::Instance().CreateFrameBuffer(frameBufferSpec);
}

void ShadowPass::Render(const FrameData &frameData)
{
    RenderPassBeginInfo beginInfo
    {
        .renderPass = renderPass,
        .frameBuffer = shadowFrameBuffer,
        .clearValues = 
        {
            {.type = AttachmentType::Depth, .format = ImageFormat::DEPTH32F, .floatValue = 1.0f },
        }
    };
    Renderer::Instance().CmdBeginRenderPass(beginInfo);
    
    Renderer::Instance().CmdBindPassMaterial(passMaterial);
    
    Renderer::Instance().CmdBindGraphicsPipeline(pipeline);

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
    for (uint32_t i = 0; i < 8; i++)
        frustumCenter += frustumCorners[i];
    frustumCenter /= 8.0f;

    // 计算包围球
    float radius = 0.0f;
    for (uint32_t i = 0; i < 8; i++)
    {
        float distance = glm::length(frustumCorners[i] - frustumCenter);
        radius = glm::max(radius, distance);
    }

    vec3 eye = frustumCenter - glm::normalize(frameData.directionalLight.direction) * radius;

    vec3 up = glm::dot(frameData.directionalLight.direction, glm::vec3(0.0f, 1.0f, 0.0f)) > 0.99f ? vec3(1, 0, 0) : vec3(0, 1, 0);
    
    mat4 lightView = Maths::LookAt(eye, frustumCenter, up);
    mat4 lightProj = Maths::Ortho(-radius, radius, -radius, radius, 0, 2 * radius);
    mat4 lightViewProj = lightProj * lightView;
    Renderer::Instance().SetUniformFromPassMaterial(passMaterial, "LightViewProjection", ShaderFundamentalType::Mat4, lightViewProj);

    for (auto renderable : frameData.renderables)
    {
        Ref<MeshAsset> meshAsset = AssetManager::Instance().GetAsset(renderable.mesh).As<MeshAsset>();
    
        Renderer::Instance().CmdBindObjectShaderMaterial(renderable.index, objectShaderMaterial);
    
        auto mesh = Renderer::Instance().GetMeshFromAsset(meshAsset);
        auto meshObj = Renderer::Instance().GetMesh(mesh);
        Renderer::Instance().CmdDrawIndexed(meshObj->GetVertexBuffer(), meshObj->GetIndexBuffer());
    }

    Renderer::Instance().CmdEndRenderPass();
}

}