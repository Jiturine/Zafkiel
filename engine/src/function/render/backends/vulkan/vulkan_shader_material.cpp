#include "function/render/backends/vulkan/vulkan_shader_material.h"
#include "function/render/backends/vulkan/vulkan_image.h"
#include "function/render/backends/vulkan/vulkan_texture.h"
#include "function/render/renderer.h"

namespace Zafkiel
{

VulkanShaderMaterialBackend::VulkanShaderMaterialBackend(uint32_t frameCount, std::vector<vk::raii::DescriptorSet> descriptorSets, std::vector<std::optional<std::vector<Scope<UniformBuffer>>>> uniformBuffers)
    : frameCount(frameCount), descriptorSets(std::move(descriptorSets)), uniformBuffers(std::move(uniformBuffers))
{
    sampledImageDirty.resize(frameCount);
}

void VulkanShaderMaterialBackend::SetTexture2D(uint32_t binding, RenderHandle handle)
{
    for (size_t frame = 0; frame < frameCount; frame++)
    {
        sampledImageDirty[frame][binding] = true;
    }
}

void VulkanShaderMaterialBackend::SetDirty(uint32_t binding) const
{
    for (size_t frame = 0; frame < frameCount; frame++)
    {
        sampledImageDirty[frame][binding] = true;
    }
}
void VulkanShaderMaterialBackend::UnsetDirty(uint32_t frame, uint32_t binding) const
{
    sampledImageDirty[frame][binding] = false;
}
bool VulkanShaderMaterialBackend::IsDirty(uint32_t frame, uint32_t binding) const
{
    return sampledImageDirty[frame].contains(binding) && sampledImageDirty[frame][binding];
}

}
