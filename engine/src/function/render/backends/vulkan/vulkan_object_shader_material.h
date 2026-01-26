#pragma once
#include "function/render/object_shader_material.h"
#include "function/render/uniform_buffer.h"
#include <vulkan/vulkan_raii.hpp>

namespace Zafkiel
{

class VulkanObjectShaderMaterialBackend final : public ObjectShaderMaterialBackend
{
  public:
    VulkanObjectShaderMaterialBackend(uint32_t minUniformBufferOffsetAlignment, std::vector<vk::raii::DescriptorSet> descriptorSets, std::vector<std::optional<std::vector<Scope<UniformBuffer>>>> uniformBuffers)
        : minUniformBufferOffsetAlignment(minUniformBufferOffsetAlignment),
          descriptorSets(std::move(descriptorSets)), uniformBuffers(std::move(uniformBuffers)) {}

    const vk::raii::DescriptorSet &GetDescriptorSet(uint32_t index) const { return descriptorSets.at(index); }
    vk::raii::DescriptorSet &GetDescriptorSet(uint32_t index) { return descriptorSets.at(index); }

    Borrow<UniformBuffer> GetUniformBuffer(uint32_t frame, uint32_t binding) const
    {
        return Borrow(uniformBuffers[binding].value()[frame]);
    }

    uint32_t GetMinUniformBufferOffsetAlignment() const { return minUniformBufferOffsetAlignment; }

  private:
    uint32_t minUniformBufferOffsetAlignment;

    std::vector<vk::raii::DescriptorSet> descriptorSets;
    std::vector<std::optional<std::vector<Scope<UniformBuffer>>>> uniformBuffers;
};
}