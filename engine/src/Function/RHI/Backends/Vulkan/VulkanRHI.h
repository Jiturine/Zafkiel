#pragma once
#include "Function/RHI/RHI.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanQueue.h"

namespace Zafkiel 
{

class PlatformWindow;

class VulkanRHI final : public RHI
{
  public:
    VulkanRHI();

    virtual GraphicsAPI GetAPI() const override { return GraphicsAPI::Vulkan; }

    virtual RHIGraphicsContext *GetGraphicsContext() override { return device->GetGraphicsContext(); }

    vk::raii::Instance &GetInstance() { return instance; }

    VulkanDevice &GetDevice() { return *device.get(); }

    virtual void WaitIdle() override;

    virtual Ref<RHIBuffer> CreateBuffer(RHICommandList &RHICmdList, const RHIBufferDesc &desc, const void *data = nullptr) override;

    virtual Ref<RHITexture> CreateTexture(RHICommandList &RHICmdList, const RHITextureDesc &desc, Buffer data = nullptr) override;

    virtual Ref<RHIGraphicsPipeline> CreateGraphicsPipeline(const RHIGraphicsPipelineDesc &desc) override;
  
    virtual Ref<RHIVertexShader> CreateVertexShader(const Path &path) override;

    virtual Ref<RHIFragmentShader> CreateFragmentShader(const Path &path) override;
  
    virtual Ref<RHIViewport> CreateViewport(PlatformWindow *window) override;

    virtual Ref<DynamicUniformBufferContent> CreateDynamicUniformBufferContent(uint32 maxSize, const ShaderReflection::UniformBlock *uniformBlock) override;
  
    virtual ImTextureRef RegisterImGuiTexture(RHITexture *texture) override;

    virtual void UnregisterImGuiTexture(RHITexture *texture) override;

  private:
    vk::raii::Context context;

    vk::raii::Instance instance;
    Scope<VulkanDevice> device;

    std::unordered_map<RHITexture *, VkDescriptorSet> registeredImGuiTextures;
};

}