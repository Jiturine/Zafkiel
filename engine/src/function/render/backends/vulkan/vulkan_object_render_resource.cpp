#include "vulkan_object_render_resource.h"
#include "vulkan_object_render_resource_template.h"
#include "vulkan_uniform_buffer.h"
#include "vulkan_render_resource.h"
#include "vulkan_context.h"
#include "function/render/renderer.h"

namespace Zafkiel
{

constexpr size_t AlignUp(size_t value, size_t alignment) 
{
    return (value + alignment - 1) & ~(alignment - 1);
}

VulkanObjectRenderResourceBackend::VulkanObjectRenderResourceBackend(const Observer<ObjectRenderResourceTemplate> objectRenderResourceTemplate, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount)
{
    minUniformBufferOffsetAlignment = physicalDevice->GetMinUniformBufferOffsetAlignment();

    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.setDescriptorPool(*descriptorManager->GetDescriptorPool())
             .setDescriptorSetCount(frameCount);

    std::vector<vk::DescriptorSetLayout> setLayouts(frameCount, *objectRenderResourceTemplate->GetBackend().As<VulkanObjectRenderResourceTemplateBackend>()->GetDescriptorSetLayout());
    allocInfo.setSetLayouts(setLayouts);

    descriptorSets = device->GetHandle().allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < frameCount; i++)
    {
        std::unordered_map<uint32_t, Scope<UniformBuffer>> oneFrameUniformBuffers;
        for (auto &[resourceName, resourceType] : objectRenderResourceTemplate->GetParameterTypes())
        {
            uint32_t binding = objectRenderResourceTemplate->GetParameterBinding(resourceName);
            if (resourceType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
            {
                auto uniformBufferType = resourceType->As<ShaderReflection::UniformBlock>();
                uint32_t perObjectsize = uniformBufferType->GetLayout().size;
                uint32_t stride = AlignUp(perObjectsize, minUniformBufferOffsetAlignment);
                uint32_t bufferSize = stride * maxObjectNum;
                oneFrameUniformBuffers[binding] = VulkanUniformBufferFactory::Create(bufferSize, device, physicalDevice);
            }
        }
        uniformBuffers.push_back(std::move(oneFrameUniformBuffers));
    }
    for (auto &[resourceName, resourceType] : objectRenderResourceTemplate->GetParameterTypes())
    {
        uint32_t binding = objectRenderResourceTemplate->GetParameterBinding(resourceName);
        if (resourceType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBufferType = resourceType->As<ShaderReflection::UniformBlock>();
            perObjectBufferSize[binding] = uniformBufferType->GetLayout().size;
        }
    }

    std::vector<vk::WriteDescriptorSet> writeInfos;
    for (size_t i = 0; i < frameCount; i++)
    {
        for (auto &[binding, uniformBuffer] : uniformBuffers[i])
        {
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.setBuffer(*uniformBuffer->GetBackend().As<VulkanUniformBufferBackend>()->GetHandle())
                      .setOffset(0)
                      .setRange(perObjectBufferSize[binding]);
            vk::WriteDescriptorSet writeInfo;
            writeInfo.setDescriptorCount(1)
                     .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
                     .setBufferInfo(bufferInfo)
                     .setDstBinding(binding)
                     .setDstSet(descriptorSets[i]);
            writeInfos.push_back(writeInfo);
        }
    }
    device->GetHandle().updateDescriptorSets(writeInfos, {});
}

void VulkanObjectRenderResourceBackend::UploadUniform() 
{
    auto curFrame = Renderer::GetGraphicsContext().As<VulkanContext>().GetCurrentFrame();
    for (auto &[paramName, param] : objectRenderResource->GetParameters())
    {
        if (param.type->GetCategory() != ShaderReflection::ResourceTypeCategory::UniformBlock) continue;

        uint32_t binding = objectRenderResource->GetParameterBinding(paramName);
        auto uniformBufferBackend = uniformBuffers[curFrame].at(binding)->GetBackend().As<VulkanUniformBufferBackend>();
        auto uniformBufferType = param.type->As<ShaderReflection::UniformBlock>();
        uint32_t perObjectsize = uniformBufferType->GetLayout().size;
        uint32_t stride = AlignUp(perObjectsize, minUniformBufferOffsetAlignment);
        for (uint32_t i = 0; i < maxObjectNum; i++)
        {
            auto src = param.dynamicUniformBuffer.Data<uint8_t>() + i * perObjectsize;
            uniformBufferBackend->SetData(i * stride, perObjectsize, src);
        }
    }
}

Scope<ObjectRenderResource> VulkanObjectRenderResourceFactory::Create(const Path &path, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount)
{
    auto renderResourceTemplate = VulkanObjectRenderResourceTemplateFactory::Create(path, device);
    auto backend = CreateScope<VulkanObjectRenderResourceBackend>(renderResourceTemplate, device, physicalDevice, descriptorManager, frameCount);
    auto result = CreateScope<ObjectRenderResource>(std::move(renderResourceTemplate), std::move(backend));
    result->GetBackend().As<VulkanObjectRenderResourceBackend>()->AttachRenderResource(result);
    return result;
}

}