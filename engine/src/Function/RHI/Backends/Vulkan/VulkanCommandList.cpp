#include "Function/RHI/Backends/Vulkan/VulkanCommandList.h"
#include "Function/RHI/Backends/Vulkan/VulkanQueue.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanRenderPass.h"
#include "Function/RHI/Backends/Vulkan/VulkanPipeline.h"
#include "Function/RHI/Backends/Vulkan/VulkanBuffer.h"
#include "Function/RHI/Backends/Vulkan/VulkanViewport.h"

namespace Zafkiel
{

VulkanCommandContext::VulkanCommandContext(VulkanQueue &queue, VulkanDevice &device)
    : queue(queue), device(device),
      commandPool(queue.AcquireCommandPool())
{
    
}

VulkanPayload *VulkanCommandContext::GetPayload(VulkanPayloadPhase phase)
{
    if (payloads.size() == 0 || phase < currentPhase)
    {
        NewPayload();
    }

    currentPhase = phase;
    return payloads.back().get();
}

void VulkanCommandContext::NewPayload()
{
    EndPayload();
    payloads.push_back(CreateScope<VulkanPayload>());
    currentPhase = VulkanPayloadPhase::Wait;
}

void VulkanCommandContext::EndPayload()
{
    if (payloads.size() > 0)
    {
        auto payload = payloads.back().get();
        if (payload->commandBuffers.size() > 0)
        {
            auto commandBuffer = payload->commandBuffers.back();
            if (!commandBuffer->HasEnded())
            {
                if (!commandBuffer->IsOutsideRenderPass())
                {
                    Log::Warn("Forcing EndRenderPass() for submission");
                    commandBuffer->EndRenderPass();
                }
                commandBuffer->End();
            }
        }
    }
}

void VulkanCommandContext::ImageMemoryBarrier(VulkanTextureBase *texture,
                                              vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask,
                                              vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                              vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask)
{
    vk::ImageSubresourceRange range;
    range.setLayerCount(1)
         .setBaseArrayLayer(0)
         .setLevelCount(1)
         .setBaseMipLevel(0)
         .setAspectMask(ImageFormatToVulkanImageAspect(texture->GetFormat()));
    vk::ImageMemoryBarrier barrier;
    barrier.setImage(texture->GetLowLevelImage())
           .setSrcAccessMask(srcAccessMask)
           .setDstAccessMask(dstAccessMask)
           .setOldLayout(oldLayout)
           .setNewLayout(newLayout)
           .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
           .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
           .setSubresourceRange(range);

    GetCommandBuffer()->GetHandle().pipelineBarrier(
        srcStageMask, dstStageMask,
        {}, {}, {}, barrier
    );
}

VulkanCommandBuffer *VulkanCommandContext::GetCommandBuffer()
{
    auto payload = GetPayload(VulkanPayloadPhase::Execute);

    if (payload->commandBuffers.size() == 0)
    {
        PrepareNewCommandBuffer(payload);
    }

    return payload->commandBuffers.back();
}

void VulkanCommandContext::PrepareNewCommandBuffer(VulkanPayload *payload)
{
	VulkanCommandBuffer *newCommandBuffer = nullptr;

	for (uint32 i = 0; i < commandPool->commandBuffers.size(); i++)
	{
		VulkanCommandBuffer *cmdBuffer = commandPool->commandBuffers[i].get();
		if (cmdBuffer->IsAvailable())
		{
			newCommandBuffer = cmdBuffer;
			break;
		}
	}

	if (!newCommandBuffer)
	{
		newCommandBuffer = commandPool->CreateCommandBuffer();
	}

	payload->commandBuffers.push_back(newCommandBuffer);

    newCommandBuffer->Begin();
}

vk::PipelineStageFlags ImageLayoutToPipelineStage(ImageLayout layout)
{
    switch (layout)
    {
        using enum ImageLayout;
    case ColorAttachment: return vk::PipelineStageFlagBits::eColorAttachmentOutput;
    case DepthAttachment: return vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    case DepthStencilAttachment: return vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    case ShaderReadOnly: return vk::PipelineStageFlagBits::eFragmentShader;
    case PresentSrc: return vk::PipelineStageFlagBits::eBottomOfPipe;
    case Undefined: return vk::PipelineStageFlagBits::eBottomOfPipe;
    default:
        Log::Error("Unsupported Image Layout!");
        return {};
    }
}

vk::AccessFlags ImageLayoutToAccessMask(ImageLayout layout)
{
    switch (layout)
    {
        using enum ImageLayout;
    case ColorAttachment: return vk::AccessFlagBits::eColorAttachmentWrite;
    case DepthAttachment: return vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    case DepthStencilAttachment: return vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    case ShaderReadOnly: return vk::AccessFlagBits::eShaderRead;
    case PresentSrc: return vk::AccessFlagBits::eMemoryRead;
    case Undefined: return vk::AccessFlagBits::eNone;
    default:
        Log::Error("Unsupported Image Layout!");
        return {};
    }
}

void VulkanGraphicsContext::WriteBuffer(RHIBuffer *buffer, Buffer data) 
{
    static_cast<VulkanBuffer *>(buffer)->Write(*GlobalRHICmdList.get(), data);
}

void VulkanGraphicsContext::UpdateUniformBuffer(RHIBuffer *uniformBuffer, Buffer data) 
{
    auto vkUniformBuffer = static_cast<VulkanBuffer *>(uniformBuffer);

    vkUniformBuffer->Write(*GlobalRHICmdList.get(), data);
}

void VulkanGraphicsContext::BeginRenderPass(const RHIRenderPassInfo &renderPassInfo) 
{
    vk::RenderPassBeginInfo beginInfo;
    std::vector<vk::ClearValue> clearValues;
    for (auto colorAttachmentInfo : renderPassInfo.colorAttachments)
    {
        clearValues.push_back(ClearValueToVulkanType(colorAttachmentInfo.clearValue, AttachmentType::Color, colorAttachmentInfo.texture->GetFormat()));
    }
    if (renderPassInfo.depthStencilAttachment.has_value())
    {
        clearValues.push_back(ClearValueToVulkanType(renderPassInfo.depthStencilAttachment.value().clearValue, AttachmentType::DepthStencil, renderPassInfo.depthStencilAttachment.value().texture->GetFormat()));
    }
    
    VulkanRenderTargetInfo renderTargetInfo(renderPassInfo);
    auto renderPass = device.GetRenderPassManager().GetOrCreateRenderPass(renderTargetInfo);
    auto frameBuffer = device.GetRenderPassManager().GetOrCreateFrameBuffer(renderTargetInfo, renderPassInfo, *renderPass);

    beginInfo.setRenderPass(renderPass->GetHandle())
             .setRenderArea(vk::Rect2D{{0,0}, {frameBuffer->GetWidth(), frameBuffer->GetHeight()}})
             .setFramebuffer(*frameBuffer->GetHandle())
             .setClearValues(clearValues);
    
    for (auto colorAttachment : renderPassInfo.colorAttachments)
    {
        if (colorAttachment.initialLayout != ImageLayout::ColorAttachment)
        {
            auto srcAccessMask = ImageLayoutToAccessMask(colorAttachment.initialLayout);
            auto dstAccessMask = ImageLayoutToAccessMask(ImageLayout::ColorAttachment);
            auto oldLayout = ImageLayoutToVulkanType(colorAttachment.initialLayout);
            auto newLayout = ImageLayoutToVulkanType(ImageLayout::ColorAttachment);
            auto srcStage = ImageLayoutToPipelineStage(colorAttachment.initialLayout);
            auto dstStage = ImageLayoutToPipelineStage(ImageLayout::ColorAttachment);
            ImageMemoryBarrier(static_cast<VulkanTextureBase *>(colorAttachment.texture), srcAccessMask, dstAccessMask, oldLayout, newLayout, srcStage, dstStage);
        }
    }
    if (renderPassInfo.depthStencilAttachment.has_value() &&
        renderPassInfo.depthStencilAttachment.value().initialLayout != ImageLayout::DepthStencilAttachment)
    {
        auto srcAccessMask = ImageLayoutToAccessMask(renderPassInfo.depthStencilAttachment.value().initialLayout);
        auto dstAccessMask = ImageLayoutToAccessMask(ImageLayout::DepthStencilAttachment);
        auto oldLayout = ImageLayoutToVulkanType(renderPassInfo.depthStencilAttachment.value().initialLayout);
        auto newLayout = ImageLayoutToVulkanType(ImageLayout::DepthStencilAttachment);
        auto srcStage = ImageLayoutToPipelineStage(renderPassInfo.depthStencilAttachment.value().initialLayout);
        auto dstStage = ImageLayoutToPipelineStage(ImageLayout::DepthStencilAttachment);
        ImageMemoryBarrier(static_cast<VulkanTextureBase *>(renderPassInfo.depthStencilAttachment.value().texture), srcAccessMask, dstAccessMask, oldLayout, newLayout, srcStage, dstStage);
    }

    GetCommandBuffer()->BeginRenderPass(beginInfo);

    currentRenderPass = renderPass;
    currentRenderPassInfo = CreateScope<RHIRenderPassInfo>(renderPassInfo);
    currentFrameBuffer = frameBuffer;
}

void VulkanGraphicsContext::BindGraphicsPipeline(RHIGraphicsPipeline *pipeline)
{
    auto vkPipeline = static_cast<VulkanGraphicsPipeline *>(pipeline);

    currentGraphicsPipeline = vkPipeline;

    auto cmdBuf = GetCommandBuffer();

    cmdBuf->GetHandle().bindPipeline(vk::PipelineBindPoint::eGraphics, *vkPipeline->GetHandle());

    // 动态视口设置
    vk::Viewport viewport;
    viewport.setX(0.0f).setY(0.0f)
            .setWidth(currentFrameBuffer->GetWidth())
            .setHeight(currentFrameBuffer->GetHeight())
            .setMinDepth(0.0f)
            .setMaxDepth(1.0f);
    cmdBuf->GetHandle().setViewport(0, viewport);

    vk::Rect2D scissor;
    scissor.setOffset({0, 0})
           .setExtent({currentFrameBuffer->GetWidth(), currentFrameBuffer->GetHeight()});
    cmdBuf->GetHandle().setScissor(0, scissor);

    ApplyStaticUniformBuffers(pipeline->GetShaders()[ShaderStage::Vertex]);
    ApplyStaticUniformBuffers(pipeline->GetShaders()[ShaderStage::Fragment]);
}

void VulkanGraphicsContext::ApplyStaticUniformBuffers(RHIShader *shader)
{
    for (auto staticUniformBufferName : shader->GetResourceTable().staticUniformBuffers)
    {
        currentGraphicsPipeline->SetUniformBuffer(shader->GetShaderStage(), staticUniformBufferName, staticUniformBuffers[staticUniformBufferName]);
    }
}

void VulkanGraphicsContext::EndRenderPass()
{
    GetCommandBuffer()->EndRenderPass();

    for (auto colorAttachment : currentRenderPassInfo->colorAttachments)
    {
        if (colorAttachment.finalLayout != ImageLayout::ColorAttachment)
        {
            auto srcAccessMask = ImageLayoutToAccessMask(ImageLayout::ColorAttachment);
            auto dstAccessMask = ImageLayoutToAccessMask(colorAttachment.finalLayout);
            auto oldLayout = ImageLayoutToVulkanType(ImageLayout::ColorAttachment);
            auto newLayout = ImageLayoutToVulkanType(colorAttachment.finalLayout);
            auto srcStage = ImageLayoutToPipelineStage(ImageLayout::ColorAttachment);
            auto dstStage = ImageLayoutToPipelineStage(colorAttachment.finalLayout);
            ImageMemoryBarrier(static_cast<VulkanTextureBase *>(colorAttachment.texture), srcAccessMask, dstAccessMask, oldLayout, newLayout, srcStage, dstStage);
        }
    }
    if (currentRenderPassInfo->depthStencilAttachment.has_value() &&
        currentRenderPassInfo->depthStencilAttachment.value().finalLayout != ImageLayout::DepthStencilAttachment)
    {
        auto srcAccessMask = ImageLayoutToAccessMask(ImageLayout::DepthStencilAttachment);
        auto dstAccessMask = ImageLayoutToAccessMask(currentRenderPassInfo->depthStencilAttachment.value().finalLayout);
        auto oldLayout = ImageLayoutToVulkanType(ImageLayout::DepthStencilAttachment);
        auto newLayout = ImageLayoutToVulkanType(currentRenderPassInfo->depthStencilAttachment.value().finalLayout);
        auto srcStage = ImageLayoutToPipelineStage(ImageLayout::DepthStencilAttachment);
        auto dstStage = ImageLayoutToPipelineStage(currentRenderPassInfo->depthStencilAttachment.value().finalLayout);
        ImageMemoryBarrier(static_cast<VulkanTextureBase *>(currentRenderPassInfo->depthStencilAttachment.value().texture), srcAccessMask, dstAccessMask, oldLayout, newLayout, srcStage, dstStage);
    }

    currentRenderPass = nullptr;
    currentRenderPassInfo = nullptr;
    currentFrameBuffer = nullptr;
}

void VulkanGraphicsContext::DrawIndexed(RHIBuffer *vertexBuffer, RHIBuffer *indexBuffer, uint32 indexCount)
{
    auto cmdBuf = GetCommandBuffer();
    currentGraphicsPipeline->BindDescriptorSets(*cmdBuf);

    auto vkVertexBuffer = static_cast<VulkanBuffer *>(vertexBuffer);
    auto vkIndexBuffer = static_cast<VulkanBuffer *>(indexBuffer);

    cmdBuf->GetHandle().bindVertexBuffers(0, *vkVertexBuffer->GetBuffer(), 0ul);
    cmdBuf->GetHandle().bindIndexBuffer(vkIndexBuffer->GetBuffer(), 0ul, vk::IndexType::eUint32);

    if (indexCount == 0)
        cmdBuf->GetHandle().drawIndexed(vkIndexBuffer->GetSize() / sizeof(uint32), 1, 0, 0, 0);
    else
        cmdBuf->GetHandle().drawIndexed(indexCount, 1, 0, 0, 0);
}

void VulkanGraphicsContext::SetStaticUniformBuffer(const std::string &name, RHIBuffer *uniformBuffer) 
{
    staticUniformBuffers[name] = uniformBuffer;
}

void VulkanGraphicsContext::Present(RHIViewport *viewport) 
{
    auto vkViewport = static_cast<VulkanViewport *>(viewport);

    vkViewport->GetSwapchain()->Present();
}

void VulkanGraphicsContext::Finalize()
{
    EndPayload();
}

void VulkanGraphicsContext::Submit()
{
    EndPayload();
    for (auto &payload : payloads)
    {
        queue.EnqueuePayload(MoveTemp(payload));
    }
    payloads.clear();
    queue.SubmitPayloads();
}

}