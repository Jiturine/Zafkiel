#include "vulkan_buffer.h"
#include "vulkan_device.h"

namespace Zafkiel 
{
VulkanBuffer::VulkanBuffer(const VulkanBufferSpecification& spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice)
    : buffer(nullptr), memory(nullptr), size(spec.size)
{
    // 创建 Buffer
    vk::BufferCreateInfo createInfo;
    createInfo.setSize(spec.size)
              .setUsage(spec.usage)
              .setSharingMode(vk::SharingMode::eExclusive);
    buffer = device->GetHandle().createBuffer(createInfo);

    // 查询内存类型
    auto requirements = buffer.getMemoryRequirements();
    memorySize = requirements.size;
    uint32_t memoryTypeIndex = 0;

    auto properties = physicalDevice->GetHandle().getMemoryProperties();
    for (size_t i = 0; i < properties.memoryTypeCount; i++)
    {
        if ((1 << i) & requirements.memoryTypeBits && properties.memoryTypes[i].propertyFlags & spec.property)
        {
            memoryTypeIndex = i;
            break;
        }
    }
    
    // 分配内存
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.setMemoryTypeIndex(memoryTypeIndex)
             .setAllocationSize(memorySize);
    memory = device->GetHandle().allocateMemory(allocInfo);

    // 绑定内存到Buffer
    buffer.bindMemory(memory, 0);
}

}