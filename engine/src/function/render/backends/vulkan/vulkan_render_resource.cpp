#include "vulkan_render_resource.h"
#include "vulkan_image.h"
#include "vulkan_texture.h"
#include "vulkan_graphics_shader.h"

namespace Zafkiel
{

VulkanRenderResourceBackend::VulkanRenderResourceBackend(const Observer<RenderResourceTemplate> renderResourceTemplate, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount)
    : device(device), frameCount(frameCount)
{
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.setDescriptorPool(*descriptorManager->GetDescriptorPool())
             .setDescriptorSetCount(frameCount);

    std::vector<vk::DescriptorSetLayout> setLayouts(frameCount, *renderResourceTemplate->GetBackend().As<VulkanRenderResourceTemplateBackend>()->GetDescriptorSetLayout());
    allocInfo.setSetLayouts(setLayouts);

    descriptorSets = device->GetHandle().allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < frameCount; i++)
    {
        std::unordered_map<uint32_t, Scope<UniformBuffer>> oneFrameUniformBuffers;
        for (auto &[resourceName, resourceType] : renderResourceTemplate->GetSchema()->GetParameterTypes())
        {
            uint32_t binding = renderResourceTemplate->GetSchema()->GetParameterBinding(resourceName);
            if (resourceType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
            {
                uint32_t size = resourceType->As<ShaderReflection::UniformBlock>()->GetLayout().size;
                oneFrameUniformBuffers[binding] = VulkanUniformBufferFactory::Create(size, device, physicalDevice);
            }
        }
        uniformBuffers.push_back(std::move(oneFrameUniformBuffers));
    }

    // Update DescriptorSet 更新所有 UniformBuffer
    std::vector<vk::WriteDescriptorSet> writeInfos;
    for (size_t i = 0; i < frameCount; i++)
    {
        for (auto &[binding, uniformBuffer] : uniformBuffers[i])
        {
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.setBuffer(*uniformBuffer->GetBackend().As<VulkanUniformBufferBackend>()->GetHandle())
                      .setOffset(0)
                      .setRange(uniformBuffer->GetSize());
            vk::WriteDescriptorSet writeInfo;
            writeInfo.setDescriptorCount(1)
                     .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                     .setBufferInfo(bufferInfo)
                     .setDstBinding(binding)
                     .setDstSet(descriptorSets[i]);
            writeInfos.push_back(writeInfo);
        }
    }
    device->GetHandle().updateDescriptorSets(writeInfos, {});

    sampledImageDirty.resize(frameCount);
}

void VulkanRenderResourceBackend::AttachRenderResource(const Observer<RenderResource> renderResource)
{
    this->renderResource = renderResource;
}

void VulkanRenderResourceBackend::SetTexture2D(const std::string &paramName, Observer<Texture2D> tex) 
{
    uint32_t binding = renderResource->GetParameterBinding(paramName);
    sampledImages[binding] = tex;
    for (size_t frame = 0; frame < frameCount; frame++)
    {
        sampledImageDirty[frame][binding] = true;
    }
}
void VulkanRenderResourceBackend::SetDirty(const std::string &key)
{
    auto path = renderResource->GetTemplate()->GetSchema()->GetAliasPath(key);
    uint32_t binding = renderResource->GetParameterBinding(path.elems[0].name);
    for (size_t frame = 0; frame < sampledImageDirty.size(); frame++)
    {
        sampledImageDirty[frame][binding] = true;
    }
}

// 更新非UniformBuffer的，可能变化的DescriptorSet
void VulkanRenderResourceBackend::UpdateDescriptorSet(uint32_t curFrame) const
{
    std::vector<vk::WriteDescriptorSet> writeInfos;
    std::vector<vk::DescriptorImageInfo> imageInfos;
    imageInfos.reserve(renderResource->GetParameters().size()); // !!!预留足够大小，确保其不会因扩容改变地址，导致setImageInfo失效
    for (auto [binding, sampledImage] : sampledImages)
    {
        if (sampledImageDirty[curFrame][binding])
        {
            vk::DescriptorImageInfo imageInfo;
            imageInfo.setImageView(*sampledImage->GetImage()->GetBackend().As<VulkanImageBackend>()->GetImageView(curFrame))
                     .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                     .setSampler(*sampledImage->GetBackend().As<VulkanTexture2DBackend>()->GetSampler());
            imageInfos.push_back(imageInfo);
            vk::WriteDescriptorSet writeInfo;
            writeInfo.setDescriptorCount(1)
                     .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                     .setImageInfo(imageInfos.back())
                     .setDstBinding(binding)
                     .setDstSet(descriptorSets[curFrame]);
            writeInfos.push_back(writeInfo);

            sampledImageDirty[curFrame][binding] = false;           
        }
    }
    if (!writeInfos.empty()) device->GetHandle().updateDescriptorSets(writeInfos, {});
}

void VulkanRenderResourceBackend::UploadUniform(size_t curFrame) const
{
    for (auto &[paramName, param] : renderResource->GetParameters())
    {
        if (param.type->GetCategory() != ShaderReflection::ResourceTypeCategory::UniformBlock) continue;

        uint32_t binding = renderResource->GetParameterBinding(paramName);
        auto uniformBufferBackend = uniformBuffers[curFrame].at(binding)->GetBackend().As<VulkanUniformBufferBackend>();
        auto src = param.uniformBuffer.Data<uint8_t>();
        auto size = param.uniformBuffer.Size<uint8_t>();
        uniformBufferBackend->SetData(0, size, src);
    }
}

Scope<RenderResource> VulkanRenderResourceFactory::Create(const Observer<RenderResourceTemplate> renderResourceTemplate, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount)
{
    auto backend = CreateScope<VulkanRenderResourceBackend>(renderResourceTemplate, device, physicalDevice, descriptorManager, frameCount);
    auto renderResource = CreateScope<RenderResource>(renderResourceTemplate, std::move(backend));
    renderResource->GetBackend().As<VulkanRenderResourceBackend>()->AttachRenderResource(renderResource);
    return renderResource;
}

}
