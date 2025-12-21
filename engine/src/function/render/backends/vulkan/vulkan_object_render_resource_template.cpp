#include "vulkan_object_render_resource_template.h"

namespace Zafkiel
{

void VulkanObjectRenderResourceTemplateBackend::InitTemplate(Observer<ObjectRenderResourceTemplate> renderResourceTemplate)
{
    auto &paramTypes = renderResourceTemplate->GetParameterTypes();
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    for (auto &[paramName, paramType] : paramTypes)
    {
        uint32_t bindingIndex = renderResourceTemplate->GetParameterBinding(paramName);
        if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            vk::DescriptorSetLayoutBinding binding;
            binding.setBinding(bindingIndex)
                   .setDescriptorCount(1)
                   .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
                   .setStageFlags(vk::ShaderStageFlagBits::eAll);
            bindings.push_back(binding);
        }
    }

    vk::DescriptorSetLayoutCreateInfo setLayoutCreateInfo;
    setLayoutCreateInfo.setBindings(bindings);

    descriptorSetLayout = device->GetHandle().createDescriptorSetLayout(setLayoutCreateInfo);
}

Scope<ObjectRenderResourceTemplate> VulkanObjectRenderResourceTemplateFactory::Create(const Path &path, const Observer<VulkanDevice> device)
{
    auto backend = CreateScope<VulkanObjectRenderResourceTemplateBackend>(device);
    auto res = CreateScope<ObjectRenderResourceTemplate>(path, std::move(backend));
    res->GetBackend().As<VulkanObjectRenderResourceTemplateBackend>()->InitTemplate(res);
    return res;
}
  
}