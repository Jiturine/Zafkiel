#pragma once
#include "function/render/backends/vulkan/vulkan_uniform_buffer.h"
#include "function/render/backends/vulkan/vulkan_shader_material_template.h"
#include "function/render/backends/vulkan/vulkan_descriptor_manager.h"
#include "function/render/backends/vulkan/vulkan_device.h"
#include "function/render/shader_material.h"

namespace Zafkiel
{

class VulkanShaderMaterialBackend final : public ShaderMaterialBackend
{
  public:
    VulkanShaderMaterialBackend(uint32_t minUniformBufferOffsetAlignment, std::vector<vk::raii::DescriptorSet> descriptorSets, std::vector<std::optional<std::vector<Scope<UniformBuffer>>>> uniformBuffers);

    vk::raii::DescriptorSet &GetDescriptorSet(size_t index) { return descriptorSets[index]; }
    const vk::raii::DescriptorSet &GetDescriptorSet(size_t index) const { return descriptorSets[index]; }

    Borrow<UniformBuffer> GetUniformBuffer(uint32_t frame, uint32_t binding) const
    {
        return Borrow(uniformBuffers[binding].value()[frame]);
    }

    virtual void SetTexture2D(uint32_t binding, RenderHandle handle) override;
    void SetDirty(uint32_t binding) const;
    void UnsetDirty(uint32_t frame, uint32_t binding) const;
    bool IsDirty(uint32_t frame, uint32_t binding) const;

  private:
    std::vector<vk::raii::DescriptorSet> descriptorSets;
    std::vector<std::optional<std::vector<Scope<UniformBuffer>>>> uniformBuffers;
    mutable std::vector<std::unordered_map<uint32_t, bool>> sampledImageDirty;
    uint32_t frameCount;
};

}