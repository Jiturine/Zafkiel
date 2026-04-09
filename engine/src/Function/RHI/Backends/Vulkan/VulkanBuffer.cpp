#include "Function/RHI/Backends/Vulkan/VulkanBuffer.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanCommandList.h"

namespace Zafkiel 
{

InternalVulkanBuffer::InternalVulkanBuffer(const InternalVulkanBufferDesc& desc, VulkanDevice &device)
    : buffer(nullptr), memory(nullptr), size(desc.size)
{
    // 创建 Buffer
    vk::BufferCreateInfo createInfo;
    createInfo.setSize(desc.size)
              .setUsage(desc.usage)
              .setSharingMode(vk::SharingMode::eExclusive);

    buffer = device.GetHandle().createBuffer(createInfo);

    // 查询内存类型
    auto requirements = buffer.getMemoryRequirements();
    memorySize = requirements.size;
    uint32 memoryTypeIndex = 0;

    auto properties = device.GetPhysicalHandle().getMemoryProperties();
    for (size_t i = 0; i < properties.memoryTypeCount; i++)
    {
        if ((1 << i) & requirements.memoryTypeBits && properties.memoryTypes[i].propertyFlags & desc.property)
        {
            memoryTypeIndex = i;
            break;
        }
    }
    
    // 分配内存
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.setMemoryTypeIndex(memoryTypeIndex)
             .setAllocationSize(memorySize);
    memory = device.GetHandle().allocateMemory(allocInfo);

    // 绑定内存到Buffer
    buffer.bindMemory(memory, 0);
}

VulkanBuffer::VulkanBuffer(const RHIBufferDesc &desc, RHICommandList &cmdList, VulkanDevice &device, const void *data) // TODO: data改成Buffer类
    : device(device), RHIBuffer(desc)
{
    vk::BufferUsageFlags vkBufferUsage;
    if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::VertexBuffer))
    {
        vkBufferUsage |= vk::BufferUsageFlagBits::eVertexBuffer;
    }
    if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::IndexBuffer))
    {
        vkBufferUsage |= vk::BufferUsageFlagBits::eIndexBuffer;
    }
    if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::UniformBuffer))
    {
        vkBufferUsage |= vk::BufferUsageFlagBits::eUniformBuffer;
    }
    if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::CPUAccessible))
    {
        vkBufferUsage |= vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
    }
    
    vk::MemoryPropertyFlags vkMemoryProperty;
    
    if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::Dynamic))
    {
        vkMemoryProperty |= vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    }
    else if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::Static))
    {
        vkMemoryProperty |= vk::MemoryPropertyFlagBits::eDeviceLocal;
    }
    else 
    {
        Log::Error("Unknown buffer memory property!");
    }

    InternalVulkanBufferDesc deviceBufferDesc
    {
        .size = desc.size,
        .usage = vkBufferUsage,
        .property = vkMemoryProperty,
    };

    buffer = CreateScope<InternalVulkanBuffer>(deviceBufferDesc, device);

    if (data)
    {
        Write(cmdList, Buffer(static_cast<const uint8*>(data), desc.size));
    }
}

void VulkanBuffer::Write(RHICommandList &cmdList, Buffer data)
{
    if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::Static))
    {
        InternalVulkanBufferDesc stagingBufferDesc
        {
            .size = data.Size<uint8>(),
            .usage = vk::BufferUsageFlagBits::eTransferSrc,
            .property = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        };
        auto stagingBuffer = CreateScope<InternalVulkanBuffer>(stagingBufferDesc, device);

        void *ptr = stagingBuffer->memory.mapMemory(0, data.Size<uint8>());
        memcpy(ptr, data.Data<uint8>(), data.Size<uint8>());
        stagingBuffer->memory.unmapMemory();

        // 直接执行命令，不使用lambda，避免生命周期问题
        auto gfxContext = static_cast<VulkanGraphicsContext *>(cmdList.GetGraphicsContext());
        auto cmdBuf = gfxContext->GetCommandBuffer();

        vk::MemoryBarrier barrierBefore;
        barrierBefore.setSrcAccessMask(vk::AccessFlagBits::eMemoryWrite);
        barrierBefore.setDstAccessMask(vk::AccessFlagBits::eMemoryRead);

        cmdBuf->GetHandle().pipelineBarrier(
            vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::PipelineStageFlagBits::eTransfer,
            {}, barrierBefore, {}, {}
        );

        vk::BufferCopy region;
        region.setSize(data.Size<uint8>())
            .setSrcOffset(0)
            .setDstOffset(0);
        cmdBuf->GetHandle().copyBuffer(stagingBuffer->buffer, buffer->buffer, region);

        vk::MemoryBarrier barrierAfter;
        barrierAfter.setSrcAccessMask(vk::AccessFlagBits::eMemoryWrite);
        barrierAfter.setDstAccessMask(vk::AccessFlagBits::eHostRead);

        cmdBuf->GetHandle().pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eHost,
            {}, barrierAfter, {}, {}
        );

        // stagingBuffer在这里析构，但命令已经提交到GPU
        // SubmitAndWaitIdle会等待GPU完成执行
        cmdList.SubmitAndWaitIdle();
    }
    else if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::Dynamic))
    {
        void *ptr = buffer->memory.mapMemory(0, data.Size<uint8>());
        memcpy(ptr, data.Data<uint8>(), data.Size<uint8>());
        buffer->memory.unmapMemory();
    }
    else
    {
        Log::Error("Unknown Buffer Usage!");
    }
}


}
