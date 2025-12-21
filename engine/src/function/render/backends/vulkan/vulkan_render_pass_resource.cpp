#include "vulkan_render_pass_resource.h"
#include "vulkan_render_resource.h"

namespace Zafkiel 
{

Scope<RenderPassResource> VulkanRenderPassResourceFactory::Create(const Path &path, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount)
{
    auto schema = CreateScope<RenderResourceSchema>(path);
    auto renderResourceTemplate = VulkanRenderResourceTemplateFactory::Create(schema, device);
    auto renderResource = VulkanRenderResourceFactory::Create(renderResourceTemplate, device, physicalDevice, descriptorManager, frameCount);
    auto result = CreateScope<RenderPassResource>(std::move(schema), std::move(renderResourceTemplate), std::move(renderResource));
    return result;
}

}