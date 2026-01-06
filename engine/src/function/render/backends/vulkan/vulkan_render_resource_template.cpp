#include "vulkan_render_resource_template.h"

namespace Zafkiel
{

void VulkanRenderResourceTemplateBackend::InitTemplate(Observer<RenderResourceTemplate> renderResourceTemplate)
{
    auto &paramTypes = renderResourceTemplate->GetSchema()->GetParameterTypes();
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    for (auto &[paramName, paramType] : paramTypes)
    {
        uint32_t bindingIndex = renderResourceTemplate->GetSchema()->GetParameterBinding(paramName);
        if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            vk::DescriptorSetLayoutBinding binding;
            binding.setBinding(bindingIndex)
                   .setDescriptorCount(1)
                   .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                   .setStageFlags(vk::ShaderStageFlagBits::eAll);
            bindings.push_back(binding);
        }
        else if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            vk::DescriptorSetLayoutBinding binding;
            binding.setBinding(bindingIndex)
                   .setDescriptorCount(1)
                   .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                   .setStageFlags(vk::ShaderStageFlagBits::eAll);
            bindings.push_back(binding);
        }
    }

    vk::DescriptorSetLayoutCreateInfo setLayoutCreateInfo;
    setLayoutCreateInfo.setBindings(bindings);

    descriptorSetLayout = device->GetHandle().createDescriptorSetLayout(setLayoutCreateInfo);
}

Scope<RenderResourceTemplate> VulkanRenderResourceTemplateFactory::Create(const Observer<RenderResourceSchema> schema, const Observer<VulkanDevice> device)
{
    auto backend = CreateScope<VulkanRenderResourceTemplateBackend>(device);
    auto res = CreateScope<RenderResourceTemplate>(schema, std::move(backend));
    res->GetBackend().As<VulkanRenderResourceTemplateBackend>()->InitTemplate(res);
    return res;
}

}
