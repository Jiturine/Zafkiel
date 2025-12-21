#include "vulkan_frame_buffer.h"
#include "vulkan_image.h"
#include "vulkan_render_pass.h"

namespace Zafkiel 
{

VulkanFrameBufferBackend::VulkanFrameBufferBackend(const FrameBufferSpecification &spec, const Scope<VulkanDevice> &device)
    : renderPass(spec.renderPass), device(device)
{
    for (auto attachment : spec.attachments)
    {
        attachments.push_back(attachment);
    }
    Invalidate(spec.width, spec.height);
}

void VulkanFrameBufferBackend::Resize(uint32_t width, uint32_t height)
{
    Invalidate(width, height);
}

void VulkanFrameBufferBackend::Invalidate(uint32_t width, uint32_t height)
{
    frameBuffers.clear();
    for (size_t i = 0; i < attachments[0]->GetBackend().As<VulkanImageBackend>()->GetImageCount(); i++)
    {
        std::vector<vk::ImageView> attachmentImageViews;
        for (size_t j = 0; j < attachments.size(); j++)
        {
            auto &attachment = attachments[j];
            auto &imageView = attachment->GetBackend().As<VulkanImageBackend>()->GetImageView(i);
            attachmentImageViews.push_back(*imageView);
        }
        vk::FramebufferCreateInfo createInfo;
        createInfo.setAttachments(attachmentImageViews)
                  .setWidth(width)
                  .setHeight(height)
                  .setRenderPass(*renderPass->GetBackend().As<VulkanRenderPassBackend>()->GetHandle())
                  .setLayers(1);
        frameBuffers.push_back(device->GetHandle().createFramebuffer(createInfo));   
    }
}


Scope<FrameBuffer> VulkanFrameBufferFactory::Create(const FrameBufferSpecification &spec, const Scope<VulkanDevice> &device)
{
    auto backend = CreateScope<VulkanFrameBufferBackend>(spec, device);
    return CreateScope<FrameBuffer>(spec, std::move(backend));
}


}