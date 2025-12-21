#pragma once
#include "function/render/render_resource_template.h"
#include "vulkan_device.h"

namespace Zafkiel 
{

class VulkanRenderResourceTemplateBackend final : public RenderResourceTemplateBackend
{
  public:
    VulkanRenderResourceTemplateBackend(const Observer<VulkanDevice> device) 
        : device(device), descriptorSetLayout(nullptr) {}
    void InitTemplate(Observer<RenderResourceTemplate> renderResourceTemplate);
    
    const vk::raii::DescriptorSetLayout &GetDescriptorSetLayout() const { return descriptorSetLayout; }

  private:
    vk::raii::DescriptorSetLayout descriptorSetLayout;
    const Observer<VulkanDevice> device;
};

class VulkanRenderResourceTemplateFactory
{
  public:
    static Scope<RenderResourceTemplate> Create(const Observer<RenderResourceSchema> schema, const Observer<VulkanDevice> device);
};

}