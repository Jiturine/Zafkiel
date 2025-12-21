#include "vulkan_command.h"
#include "vulkan_object_render_resource.h"
#include "vulkan_render_resource.h"
#include "vulkan_vertex_buffer.h"
#include "vulkan_index_buffer.h"
#include "vulkan_context.h"
#include "vulkan_material.h"
#include "vulkan_render_pass.h"
#include "vulkan_pipeline.h"

namespace Zafkiel
{

static constexpr uint32_t GlobalRenderResourceSet = 0;
static constexpr uint32_t RenderPassResourceSet = 1;
static constexpr uint32_t MaterialSet = 2;
static constexpr uint32_t ObjectRenderResourceSet = 3;

constexpr size_t AlignUp(size_t value, size_t alignment) 
{
    return (value + alignment - 1) & ~(alignment - 1);
}

static vk::ClearValue ClearValueToVulkanType(ClearValue value)
{
    vk::ClearValue result;
    if (value.type == AttachmentType::Color)
    {
        switch (value.format)
        {
            using enum ImageFormat;
        case R8:
        case R8_sRGB:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.floatValue, 0, 0, 0}));
            break;
        case RG8:
        case RG8_sRGB:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.vec2Value.x, value.vec2Value.y, 0, 0}));
            break;
        case BGR8:
        case RGB8:
        case RGB8_sRGB:
        case BGR8_sRGB:
        case RGB16F:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.vec3Value.x, value.vec3Value.y, value.vec3Value.z, 0}));
            break;
        case BGRA8:
        case RGBA8:
        case RGBA8_sRGB:
        case BGRA8_sRGB:
        case RGBA16F:
        case RGBA32F:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.vec4Value.x, value.vec4Value.y, value.vec4Value.z, value.vec4Value.w}));
            break;
        case R32UI:
            result.setColor(vk::ClearColorValue(std::array<uint32_t, 4>{value.uintValue, 0, 0, 0}));
            break;
        default:
            Log::Error("Unsupported Clear Color Value!");
            break;
        }
    }
    else if (value.type == AttachmentType::DepthStencil)
    {
        result.setDepthStencil(vk::ClearDepthStencilValue{value.floatValue, value.uintValue});
    }
    else if (value.type == AttachmentType::Depth)
    {
        result.setDepthStencil(vk::ClearDepthStencilValue{value.floatValue, {}});
    }
    else if (value.type == AttachmentType::Stencil)
    {
        result.setDepthStencil(vk::ClearDepthStencilValue{{}, value.uintValue});
    }
    else
    {
        Log::Error("Unknown ClearValue Format!");
    }
    return result;
}


void VulkanCommand::ImageMemoryBarrier(Observer<Image> image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    vk::AccessFlagBits srcAccess, vk::AccessFlagBits dstAccess, vk::PipelineStageFlagBits srcStage, vk::PipelineStageFlagBits dstStage)
{
    vk::ImageSubresourceRange range;
    range.setAspectMask(vk::ImageAspectFlagBits::eColor)
         .setBaseMipLevel(0)
         .setLevelCount(1)
         .setBaseArrayLayer(0)
         .setLayerCount(1);

    vk::ImageMemoryBarrier barrier;
    barrier.setOldLayout(oldLayout)
           .setNewLayout(newLayout)
           .setSrcAccessMask(srcAccess)
           .setDstAccessMask(dstAccess)
           .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
           .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
           .setImage(*image->GetBackend().As<VulkanImageBackend>()->GetImage(curFrame))
           .setSubresourceRange(range);
    commandBuffer.pipelineBarrier(
        srcStage,
        dstStage,
        {},     
        nullptr, nullptr, barrier
    );
}

void VulkanCommand::BeginRenderPassImpl(const RenderPassBeginInfo &beginInfo)
{
    auto vulkanRenderPass = beginInfo.renderPass->GetBackend().As<VulkanRenderPassBackend>();
    auto frameBuffer = beginInfo.frameBuffer;
    auto frameBufferBackend = frameBuffer->GetBackend().As<VulkanFrameBufferBackend>();
    vk::RenderPassBeginInfo vkBeginInfo;
    std::vector<vk::ClearValue> vkClearValues;
    for (auto &clearValue : beginInfo.clearValues)
    {
        vkClearValues.push_back(ClearValueToVulkanType(clearValue));
    }
    vkBeginInfo.setRenderPass(vulkanRenderPass->GetHandle())
               .setRenderArea(vk::Rect2D{{0,0}, {frameBuffer->GetWidth(), frameBuffer->GetHeight()}})
               .setFramebuffer(*frameBufferBackend->GetFrameBuffer(curFrame))
               .setClearValues(vkClearValues);
    commandBuffer.beginRenderPass(vkBeginInfo, vk::SubpassContents::eInline);

    currentFrameBuffer = frameBuffer;
}
void VulkanCommand::EndRenderPassImpl()
{
    commandBuffer.endRenderPass();
    currentFrameBuffer = nullptr;
}
void VulkanCommand::BindPipelineImpl(const Observer<Pipeline> pipeline)
{
    currentPipeline = pipeline;
    auto bindPoint = PipelineTypeToVulkanBindPoint(pipeline->GetPipelineType());
    auto pipelineBackend = pipeline->GetPipelineBackend().As<VulkanPipelineBackend>();
    commandBuffer.bindPipeline(bindPoint, *pipelineBackend->GetPipeline());

    if (globalRenderResourceChanged)
    {
        BindGlobalRenderResourceInPipeline();
        globalRenderResourceChanged = false;
    }
    if (renderPassResourceChanged)
    {
        BindRenderPassResourceInPipeline();
        renderPassResourceChanged = false;
    }

    // 动态视口设置
    vk::Viewport viewport;
    viewport.setX(0.0f).setY(0.0f)
            .setWidth(currentFrameBuffer->GetWidth())
            .setHeight(currentFrameBuffer->GetHeight())
            .setMinDepth(0.0f)
            .setMaxDepth(1.0f);
    vk::Rect2D scissor;
    scissor.setOffset({0, 0})
           .setExtent({currentFrameBuffer->GetWidth(), currentFrameBuffer->GetHeight()});
    commandBuffer.setViewport(0, viewport, {});
    commandBuffer.setScissor(0, scissor, {});
}
void VulkanCommand::BindGlobalRenderResourceImpl(const Observer<GlobalRenderResource> globalRenderResource)
{
    globalRenderResourceChanged = true;
    currentGlobalRenderResource = globalRenderResource;
}
void VulkanCommand::BindRenderPassResourceImpl(const Observer<RenderPassResource> renderPassResource)
{
    renderPassResourceChanged = true;
    currentRenderPassResource = renderPassResource;
}
void VulkanCommand::BindGlobalRenderResourceInPipeline()
{
    auto backend = currentGlobalRenderResource->GetRenderResource()->GetBackend().As<VulkanRenderResourceBackend>();
    backend->UpdateDescriptorSet(curFrame);
    backend->UploadUniform(curFrame);

    auto pipeline = currentPipeline->GetPipelineBackend().As<VulkanPipelineBackend>();
    auto bindPoint = PipelineTypeToVulkanBindPoint(currentPipeline->GetPipelineType());
    commandBuffer.bindDescriptorSets(bindPoint, *pipeline->GetPipelineLayout(),
        GlobalRenderResourceSet, *backend->GetDescriptorSet(curFrame), {}, {});
}
void VulkanCommand::BindRenderPassResourceInPipeline()
{
    auto backend = currentRenderPassResource->GetRenderResource()->GetBackend().As<VulkanRenderResourceBackend>();
    backend->UpdateDescriptorSet(curFrame);
    backend->UploadUniform(curFrame);

    auto pipeline = currentPipeline->GetPipelineBackend().As<VulkanPipelineBackend>();
    auto bindPoint = PipelineTypeToVulkanBindPoint(currentPipeline->GetPipelineType());
    commandBuffer.bindDescriptorSets(bindPoint, *pipeline->GetPipelineLayout(),
        RenderPassResourceSet, *backend->GetDescriptorSet(curFrame), {}, {});
}
void VulkanCommand::BindMaterialImpl(const Observer<Material> material)
{
    auto backend = material->GetRenderResource()->GetBackend().As<VulkanRenderResourceBackend>();
    backend->UpdateDescriptorSet(curFrame);
    backend->UploadUniform(curFrame);

    auto pipeline = currentPipeline->GetPipelineBackend().As<VulkanPipelineBackend>();
    auto bindPoint = PipelineTypeToVulkanBindPoint(currentPipeline->GetPipelineType());
    commandBuffer.bindDescriptorSets(bindPoint, *pipeline->GetPipelineLayout(),
        MaterialSet, *backend->GetDescriptorSet(curFrame), {}, {});
}
void VulkanCommand::BindObjectRenderResourceImpl(uint32_t index, const Observer<ObjectRenderResource> objectRenderResource)
{
    auto backend = objectRenderResource->GetBackend().As<VulkanObjectRenderResourceBackend>();
    // 在填入dynamic UBO时就upload
    std::vector<std::pair<uint32_t, uint32_t>> offsetWithBindings;
    for (auto &[paramName, paramType] : objectRenderResource->GetTemplate()->GetParameterTypes())
    {
        if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBufferType = paramType->As<ShaderReflection::UniformBlock>();
            uint32_t perObjectsize = uniformBufferType->GetLayout().size;
            uint32_t stride = AlignUp(perObjectsize, backend->GetMinUniformBufferOffsetAlignment());
            offsetWithBindings.emplace_back(
                objectRenderResource->GetTemplate()->GetParameterBinding(paramName),
                index * stride);
        }
    }
    std::sort(offsetWithBindings.begin(), offsetWithBindings.end(), [](const auto &elem1, const auto &elem2) {
        return elem1.first < elem2.first;
    });
    std::vector<uint32_t> offsets;
    for (auto [_, offset] : offsetWithBindings)
    {
        offsets.push_back(offset);
    }

    auto pipeline = currentPipeline->GetPipelineBackend().As<VulkanPipelineBackend>();
    auto bindPoint = PipelineTypeToVulkanBindPoint(currentPipeline->GetPipelineType());
    commandBuffer.bindDescriptorSets(bindPoint, *pipeline->GetPipelineLayout(),
        ObjectRenderResourceSet, *backend->GetDescriptorSet(curFrame), offsets, {});
}
void VulkanCommand::DrawIndexedImpl(const Observer<VertexBuffer> vertexBuffer, const Observer<IndexBuffer> indexBuffer)
{
    commandBuffer.bindVertexBuffers(0, *vertexBuffer->GetBackend().As<VulkanVertexBufferBackend>()->GetHandle(), 0ul);
    commandBuffer.bindIndexBuffer(indexBuffer->GetBackend().As<VulkanIndexBufferBackend>()->GetHandle(), 0ul, vk::IndexType::eUint32);
    commandBuffer.drawIndexed(indexBuffer->GetCount(), 1, 0, 0, 0);
}
void VulkanCommand::BeginFrame(const Scope<GraphicsContext> &context)
{
    commandBuffer = context.As<VulkanContext>().GetCurrentCommandBuffer();
    curFrame = context.As<VulkanContext>().GetCurrentFrame();
}

}
