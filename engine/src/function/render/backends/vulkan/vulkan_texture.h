#pragma once
#include "vulkan_device.h"
#include "vulkan_command_manager.h"
#include "function/render/texture.h"
#include "function/render/image.h"

namespace Zafkiel
{
  
vk::SamplerAddressMode TextureWrapToVulkanAddressMode(TextureWrap wrap);
vk::Filter FilterTypeToVulkanType(TextureFilter filter);
  
class VulkanTexture2DBackend final : public Texture2DBackend
{
  public:
    VulkanTexture2DBackend(const Texture2DSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager, uint32_t frameCount);
    
    void SetData(Observer<Image> image, Buffer buffer);

    const vk::raii::Sampler &GetSampler() const { return sampler; }
    vk::raii::Sampler &GetSampler() { return sampler; }

  private:
    void ExecuteCmd(std::function<void(vk::raii::CommandBuffer&)>);

    vk::raii::Sampler sampler;

    const Scope<VulkanDevice> &device;
    const Scope<VulkanPhysicalDevice> &physicalDevice;
    const Scope<VulkanCommandManager> &commandManager;
    uint32_t frameCount;
};

class VulkanTexture2DFactory final : public Texture2DFactory<VulkanTexture2DFactory>
{
  public:
    static Scope<Texture2D> Create(const Texture2DSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager, uint32_t frameCount);
    static Scope<Texture2D> Create(const Texture2DSpecification &spec, Buffer buffer, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager, uint32_t frameCount);
};

}
