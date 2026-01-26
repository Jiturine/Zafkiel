#include "function/render/backends/vulkan/vulkan_descriptor_manager.h"

namespace Zafkiel 
{
VulkanDescriptorManager::VulkanDescriptorManager(Borrow<VulkanDevice> device)
    : descriptorPool(nullptr)
{
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    constexpr uint32_t descriptorCount = 1000, maxSets = 100;
    std::vector<vk::DescriptorPoolSize> poolSizes(11);
    poolSizes[0].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eSampler);
    poolSizes[1].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eCombinedImageSampler);
    poolSizes[2].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eStorageImage);
    poolSizes[3].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eUniformTexelBuffer);
    poolSizes[4].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eStorageTexelBuffer);
    poolSizes[5].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eUniformBuffer);
    poolSizes[6].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eStorageBuffer);
    poolSizes[7].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eUniformBufferDynamic);
    poolSizes[8].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eStorageBufferDynamic);
    poolSizes[9].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eInputAttachment);
    poolSizes[10].setDescriptorCount(descriptorCount).setType(vk::DescriptorType::eSampledImage);
    descriptorPoolCreateInfo.setMaxSets(maxSets)
                            .setPoolSizes(poolSizes)
                            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    descriptorPool = device->GetHandle().createDescriptorPool(descriptorPoolCreateInfo);
}

}