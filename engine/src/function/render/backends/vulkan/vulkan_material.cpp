#include "vulkan_material.h"
#include "vulkan_image.h"
#include "vulkan_texture.h"
#include "vulkan_graphics_shader.h"
#include "vulkan_render_resource.h"

namespace Zafkiel
{
    
Scope<Material> VulkanMaterialFactory::Create(const MaterialSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount)
{
    auto renderResource = VulkanRenderResourceFactory::Create(spec.materialTemplate, device, physicalDevice, descriptorManager, frameCount);
    auto material = CreateScope<Material>(spec, std::move(renderResource));
    return material;
}

}
