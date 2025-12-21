#include "vulkan_fragment_module.h"
#include <spirv_cross/spirv_cross.hpp>

namespace Zafkiel 
{

VulkanFragmentModuleBackend::VulkanFragmentModuleBackend(Buffer codeBuffer, const Scope<VulkanDevice> &device, const Scope<VulkanShaderModuleBackend> &shaderModuleBackend)
{
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setCode({codeBuffer.Size<uint32_t>(), codeBuffer.Data<uint32_t>()});
    shaderModuleBackend->shaderModule = device->GetHandle().createShaderModule(createInfo);
}

Scope<FragmentModule> VulkanFragmentModuleFactory::Create(Buffer buffer, const Scope<VulkanDevice> &device)
{
    auto shaderModuleBackend = CreateScope<VulkanShaderModuleBackend>();
    auto fragmentModuleBackend = CreateScope<VulkanFragmentModuleBackend>(buffer, device, shaderModuleBackend);
    return CreateScope<FragmentModule>(buffer, std::move(shaderModuleBackend), std::move(fragmentModuleBackend));
}

}