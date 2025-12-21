#pragma once
#include "function/render/object_render_resource_template.h"
#include "vulkan_device.h"

namespace Zafkiel 
{

class VulkanObjectRenderResourceTemplateBackend final : public ObjectRenderResourceTemplateBackend
{
  public:
    VulkanObjectRenderResourceTemplateBackend(const Observer<VulkanDevice> device) 
        : device(device), descriptorSetLayout(nullptr) {}
    void InitTemplate(Observer<ObjectRenderResourceTemplate> renderResourceTemplate);
    
    const vk::raii::DescriptorSetLayout &GetDescriptorSetLayout() const { return descriptorSetLayout; }

  private:
    vk::raii::DescriptorSetLayout descriptorSetLayout;
    const Observer<VulkanDevice> device;
};

class VulkanObjectRenderResourceTemplateFactory
{
  public:
    static Scope<ObjectRenderResourceTemplate> Create(const Path &path, const Observer<VulkanDevice> device);
};

}