#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Function/RHI/RHIResources.h"

namespace Zafkiel
{

class VulkanDevice;
class VulkanShader;
class VulkanBuffer;
class VulkanTexture;

using DescriptorSetHashKey = uint64;

DescriptorSetHashKey CalculateDescriptorSetHashKey( VulkanShader *shader,
    const std::vector<std::optional<VulkanBuffer *>> &bufferInfos,
    const std::vector<std::optional<VulkanTexture *>> &imageInfos);

class VulkanDescriptorManager final
{
  public:
    VulkanDescriptorManager(VulkanDevice &device);

    vk::raii::DescriptorPool &GetDescriptorPool() { return descriptorPool; }

    vk::raii::DescriptorSetLayout &GetOrCreateDescriptorSetLayout(VulkanShader *shader);

    vk::raii::DescriptorSet &GetOrCreateDescriptorSet(
        VulkanShader *shader,
        const std::vector<std::optional<VulkanBuffer *>> &bufferInfos,
        const std::vector<std::optional<VulkanTexture *>> &imageInfos);

    void ClearCache();

  private:
    vk::raii::DescriptorPool descriptorPool;

    std::unordered_map<ShaderKey, std::optional<vk::raii::DescriptorSetLayout>> descriptorSetLayouts;

    std::unordered_map<DescriptorSetHashKey, std::optional<vk::raii::DescriptorSet>> descriptorSetCache;

    VulkanDevice &device;
};

}
