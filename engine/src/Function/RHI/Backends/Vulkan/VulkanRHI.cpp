#include "Function/RHI/Backends/Vulkan/VulkanRHI.h"
#include "Function/RHI/Backends/Vulkan/VulkanBuffer.h"
#include "Function/RHI/Backends/Vulkan/VulkanTexture.h"
#include "Function/RHI/Backends/Vulkan/VulkanPipeline.h"
#include "Function/RHI/Backends/Vulkan/VulkanShader.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/ShaderCompiler/GlslCompiler.h"
#include "Function/RHI/ShaderCompiler/SpirvReflection.h"

#include "Function/Window/Window.h"

#include <imgui_impl_vulkan.h>
#include <SDL3/SDL_vulkan.h>

namespace Zafkiel 
{

VulkanRHI::VulkanRHI(Window &window)
    : instance(nullptr), surface(nullptr)
{
    // 创建 vk::Instance
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_API_VERSION_1_4);

    createInfo.setPApplicationInfo(&appInfo);

    uint32 extensionCount;
    const char *const *extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);

    std::vector<const char *> fullExtensions;
    
    for (size_t i = 0; i < extensionCount; i++)
        fullExtensions.push_back(extensions[i]);
    
    fullExtensions.push_back(vk::EXTSurfaceMaintenance1ExtensionName);     // 
    fullExtensions.push_back(vk::KHRGetSurfaceCapabilities2ExtensionName); // swapchain 同步扩展 

    std::vector<const char *> layers = {"VK_LAYER_KHRONOS_validation"};
    createInfo.setPpEnabledLayerNames(layers.data())
              .setEnabledLayerCount(layers.size())
              .setEnabledExtensionCount(fullExtensions.size())
              .setPpEnabledExtensionNames(fullExtensions.data());

    instance = context.createInstance(createInfo);

    // 从SDL获取surface
    VkSurfaceKHR cStyleSurface;
    bool success = SDL_Vulkan_CreateSurface(window.GetHandle(), *instance, nullptr, &cStyleSurface);
    if (!success)
    {
        Log::Error("Error when Create Surface: {}", SDL_GetError());
    }
    surface = { instance, cStyleSurface };

    // 选择物理设备
    auto physicalDevices = instance.enumeratePhysicalDevices();
        
    device = CreateScope<VulkanDevice>(physicalDevices[0], surface, *this);
}

void VulkanRHI::WaitIdle()
{
    device->GetHandle().waitIdle();
}

Ref<RHIBuffer> VulkanRHI::CreateBuffer(RHICommandList &RHICmdList, const RHIBufferDesc &desc, const void *data) 
{
    return CreateRef<VulkanBuffer>(desc, RHICmdList, *device.get(), data);
}

Ref<RHITexture> VulkanRHI::CreateTexture(RHICommandList &RHICmdList, const RHITextureDesc &desc, Buffer data)
{
    return CreateRef<VulkanTexture>(desc, RHICmdList, *device.get(), data);
}

Ref<RHIGraphicsPipeline> VulkanRHI::CreateGraphicsPipeline(const RHIGraphicsPipelineDesc &desc) 
{
    return CreateRef<VulkanGraphicsPipeline>(desc, *device.get());
}

Ref<RHIVertexShader> VulkanRHI::CreateVertexShader(const Path &path)
{
    std::string source = FileSystem::ReadText(path);

    GlslCompiler compiler(path.parent_path());

    ScopedBuffer code = compiler.Compile(source, GraphicsAPI::Vulkan, ShaderType::Vertex, path.filename().stem().string());

    return device->GetShaderRegistry().CreateShader<VulkanVertexShader>(code, *device.get(), compiler.GetReflection());
}

Ref<RHIFragmentShader> VulkanRHI::CreateFragmentShader(const Path &path)
{
    std::string source = FileSystem::ReadText(path);

    GlslCompiler compiler(path.parent_path());

    ScopedBuffer code = compiler.Compile(source, GraphicsAPI::Vulkan, ShaderType::Fragment, path.filename().stem().string());

    return device->GetShaderRegistry().CreateShader<VulkanFragmentShader>(code, *device.get(), compiler.GetReflection());
}

Ref<DynamicUniformBufferContent> VulkanRHI::CreateDynamicUniformBufferContent(uint32 maxSize, const ShaderReflection::UniformBlock *uniformBlock) 
{
    uint32 alignment = device->GetMinUniformBufferOffsetAlignment();

    return CreateRef<DynamicUniformBufferContent>(maxSize, uniformBlock, alignment);
}

ImTextureRef VulkanRHI::RegisterImGuiTexture(RHITexture *texture) 
{
    device->GetHandle().waitIdle();

    auto vkTexture = static_cast<VulkanTexture *>(texture);

    auto descriptorSet = ImGui_ImplVulkan_AddTexture(*vkTexture->GetSampler(), *vkTexture->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    registeredImGuiTextures[texture] = descriptorSet;
    
    return descriptorSet;
}

void VulkanRHI::UnregisterImGuiTexture(RHITexture *texture) 
{
    auto vkTexture = static_cast<VulkanTexture *>(texture);
    
    // ImGui_ImplVulkan_RemoveTexture 会导致vkFreeDescriptorSets的validation Error
    // pDescriptorSets[0] can't be called on VkDescriptorSet 0x970000000097 that is currently in use by VkCommandBuffer 0x7fffe998b140
    // 因此必须waitIdle
    GlobalRHICmdList->SubmitAndWaitIdle();

    ImGui_ImplVulkan_RemoveTexture(registeredImGuiTextures[texture]);
}

}