#include "Function/RHI/Backends/Vulkan/VulkanDescriptorManager.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanBuffer.h"

namespace Zafkiel 
{
VulkanDescriptorManager::VulkanDescriptorManager(VulkanDevice &device)
    : descriptorPool(nullptr), device(device)
{
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    constexpr uint32 descriptorCount = 1000, maxSets = 100;
    
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

    descriptorPool = device.GetHandle().createDescriptorPool(descriptorPoolCreateInfo);
}

DescriptorSetHashKey CalculateDescriptorSetHashKey( VulkanShader *shader,
    const std::vector<std::optional<VulkanBuffer *>> &bufferInfos,
    const std::vector<std::optional<VulkanTexture *>> &imageInfos)
{
    uint64 hash = shader->GetShaderKey();
    std::vector<uint64> resourceHashes;

    auto &SRT = shader->GetShaderResourceTable();
    
    for (auto &bufferInfo : bufferInfos)
    {
        if (!bufferInfo.has_value()) continue;

        VkBuffer vkBuffer = *bufferInfo.value()->GetBuffer();

        resourceHashes.push_back(reinterpret_cast<uint64>(vkBuffer));
    }

    for (auto imageInfo : imageInfos)
    {
        if (!imageInfo.has_value()) continue;

        VkImageView imageView = *imageInfo.value()->GetImageView();
        VkSampler sampler = *imageInfo.value()->GetSampler();

        uint64 resourceHash = reinterpret_cast<uint64>(imageView);
        resourceHash ^= reinterpret_cast<uint64>(sampler);

        resourceHashes.push_back(resourceHash);
    }

    for (auto h : resourceHashes)
    {
        hash ^= h + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }

    return hash;
}


vk::raii::DescriptorSetLayout &VulkanDescriptorManager::GetOrCreateDescriptorSetLayout(VulkanShader *shader)
{
    if (descriptorSetLayouts.contains(shader->GetShaderKey()))
    {
        return descriptorSetLayouts[shader->GetShaderKey()].value();
    }

    auto &SRT = shader->GetShaderResourceTable();
    auto &paramTypeInfos = SRT.resourceTypeInfos;
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    for (auto [binding, resourceTypeInfo] : std::views::enumerate(paramTypeInfos))
    {
        if (resourceTypeInfo.type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            vk::DescriptorSetLayoutBinding layoutBinding;
            layoutBinding.setBinding(binding)
                         .setDescriptorCount(1)
                         .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
                         .setStageFlags(vk::ShaderStageFlagBits::eAll);
            bindings.push_back(layoutBinding);
        }
        else if (resourceTypeInfo.type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            vk::DescriptorSetLayoutBinding layoutBinding;
            layoutBinding.setBinding(binding)
                         .setDescriptorCount(1)
                         .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                         .setStageFlags(vk::ShaderStageFlagBits::eAll);
            bindings.push_back(layoutBinding);
        }
    }

    vk::DescriptorSetLayoutCreateInfo setLayoutCreateInfo;
    setLayoutCreateInfo.setBindings(bindings);

    auto descriptorSetLayout = device.GetHandle().createDescriptorSetLayout(setLayoutCreateInfo);

    descriptorSetLayouts[shader->GetShaderKey()] = MoveTemp(descriptorSetLayout);

    return descriptorSetLayouts[shader->GetShaderKey()].value();
}

vk::raii::DescriptorSet &VulkanDescriptorManager::GetOrCreateDescriptorSet(
    VulkanShader *shader,
    const std::vector<std::optional<VulkanBuffer *>> &bufferInfos,
    const std::vector<std::optional<VulkanTexture *>> &imageInfos)
{
    DescriptorSetHashKey hashKey = CalculateDescriptorSetHashKey(shader, bufferInfos, imageInfos);

    if (descriptorSetCache.contains(hashKey))
    {
        return descriptorSetCache[hashKey].value();
    }

    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.setDescriptorPool(*descriptorPool)
             .setDescriptorSetCount(1)
             .setSetLayouts(*GetOrCreateDescriptorSetLayout(shader));

    auto descriptorSets = device.GetHandle().allocateDescriptorSets(allocInfo);

    descriptorSetCache[hashKey] = MoveTemp(descriptorSets[0]);

    return descriptorSetCache[hashKey].value();
}

void VulkanDescriptorManager::ClearCache()
{
    descriptorSetCache.clear();
}

}