#pragma once
#include "vulkan_uniform_buffer.h"
#include "vulkan_render_resource_template.h"
#include "vulkan_descriptor_manager.h"
#include "vulkan_device.h"
#include "function/render/render_resource.h"

namespace Zafkiel
{

class VulkanRenderResourceBackend final : public RenderResourceBackend
{
  public:
    VulkanRenderResourceBackend(const Observer<RenderResourceTemplate> renderResourceTemplate, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount);

    void UpdateDescriptorSet(uint32_t curFrame) const;

    vk::raii::DescriptorSet &GetDescriptorSet(size_t index) { return descriptorSets[index]; }
    const vk::raii::DescriptorSet &GetDescriptorSet(size_t index) const { return descriptorSets[index]; }

    void UploadUniform(size_t curFrame) const;

    virtual void SetTexture2D(const std::string &paramName, Observer<Texture2D> tex) override;
    virtual void SetDirty(const std::string &key) override;

    friend class VulkanRenderResourceFactory;

  private:
    void AttachRenderResource(const Observer<RenderResource> renderResource);
  
    std::vector<vk::raii::DescriptorSet> descriptorSets;
  
    std::vector<std::unordered_map<uint32_t, Scope<UniformBuffer>>> uniformBuffers;
  
    std::unordered_map<uint32_t, Observer<Texture2D>> sampledImages;
    mutable std::vector<std::unordered_map<uint32_t, bool>> sampledImageDirty;

    const Scope<VulkanDevice> &device;
    Observer<RenderResource> renderResource;
    uint32_t frameCount;
};

class VulkanRenderResourceFactory final 
{
  public:
    static Scope<RenderResource> Create(const Observer<RenderResourceTemplate> renderResourceTemplate, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount);
};

}