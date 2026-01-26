#pragma once
#include "function/render/backends/vulkan/vulkan_device.h"
#include "function/render/backends/vulkan/vulkan_command_manager.h"
#include "function/render/texture.h"
#include "function/render/image.h"
#include "function/render/backends/vulkan/vulkan_image.h"

namespace Zafkiel
{

vk::SamplerAddressMode TextureWrapToVulkanAddressMode(TextureWrap wrap);
vk::Filter FilterTypeToVulkanType(TextureFilter filter);

class VulkanTexture2DBackend final : public Texture2DBackend
{
  public:
    VulkanTexture2DBackend(vk::raii::Sampler sampler) : sampler(std::move(sampler)) {}

    const vk::raii::Sampler &GetSampler() const { return sampler; }
    vk::raii::Sampler &GetSampler() { return sampler; }

  private:
    vk::raii::Sampler sampler;
    uint32_t frameCount;
};

}
