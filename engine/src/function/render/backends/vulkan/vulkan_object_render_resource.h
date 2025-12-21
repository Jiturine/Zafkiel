#pragma once
#include "function/render/object_render_resource.h"
#include "vulkan_descriptor_manager.h"

namespace Zafkiel
{

class VulkanObjectRenderResourceBackend final : public ObjectRenderResourceBackend
{
  public:
    VulkanObjectRenderResourceBackend(const Observer<ObjectRenderResourceTemplate> objectRenderResourceTemplate, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount);

    const vk::raii::DescriptorSet &GetDescriptorSet(uint32_t index) const { return descriptorSets.at(index); }
    vk::raii::DescriptorSet &GetDescriptorSet(uint32_t index) { return descriptorSets.at(index); }
    
    virtual void UploadUniform() override;

    void AttachRenderResource(const Observer<ObjectRenderResource> objectRenderResource) { this->objectRenderResource = objectRenderResource; }

    uint32_t GetMinUniformBufferOffsetAlignment() const { return minUniformBufferOffsetAlignment; }

  private:
    uint32_t minUniformBufferOffsetAlignment;

    std::vector<vk::raii::DescriptorSet> descriptorSets;

    std::vector<std::unordered_map<uint32_t, Scope<UniformBuffer>>> uniformBuffers;
    std::unordered_map<uint32_t, uint32_t> perObjectBufferSize; // binding -> size
    Observer<ObjectRenderResource> objectRenderResource;
    uint32_t frameCount;
};

class VulkanObjectRenderResourceFactory
{
  public:
    static Scope<ObjectRenderResource> Create(const Path &path, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount);
};

}