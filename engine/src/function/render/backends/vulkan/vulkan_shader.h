#pragma once
#include "function/render/shader.h"
#include "function/render/shader_module.h"
#include "function/render/backends/vulkan/vulkan_device.h"
#include "function/render/backends/vulkan/vulkan_descriptor_manager.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{

vk::ShaderStageFlagBits ShaderStageToVulkanType(ShaderStage stage);

class VulkanShaderBackend final : public ShaderBackend
{
};

}
