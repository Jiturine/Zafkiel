#include "vulkan_vertex_module.h"

namespace Zafkiel
{

VulkanVertexModuleBackend::VulkanVertexModuleBackend(Buffer codeBuffer, const Scope<VulkanDevice> &device, const Scope<VulkanShaderModuleBackend> &shaderModuleBackend)
{
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setCode({codeBuffer.Size<uint32_t>(), codeBuffer.Data<uint32_t>()});
    shaderModuleBackend->shaderModule = device->GetHandle().createShaderModule(createInfo);
}

Scope<VertexModule> VulkanVertexModuleFactory::Create(Buffer codeBuffer, const Scope<VulkanDevice> &device)
{
    auto shaderModuleBackend = CreateScope<VulkanShaderModuleBackend>();
    auto vertexModuleBackend = CreateScope<VulkanVertexModuleBackend>(codeBuffer, device, shaderModuleBackend);
    return CreateScope<VertexModule>(codeBuffer, std::move(shaderModuleBackend), std::move(vertexModuleBackend));
}

}
