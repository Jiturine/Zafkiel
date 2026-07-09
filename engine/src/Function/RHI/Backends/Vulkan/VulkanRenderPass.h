#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Function/RHI/RHIResources.h"

namespace Zafkiel
{

class VulkanDevice;

vk::ClearValue ClearValueToVulkanType(ClearValue value, AttachmentType attachType, ImageFormat attachFormat);

struct VulkanRenderPassCompatibleHashStruct
{
    VulkanRenderPassCompatibleHashStruct()
    {
        memset(this, 0, sizeof(VulkanRenderPassCompatibleHashStruct));
    };
    uint8 attachmentCount;
    uint8 sampleCount;
    ImageFormat formats[MaxRenderTargets];
};

struct VulkanRenderPassFullHashStruct
{
    VulkanRenderPassFullHashStruct()
    {
        memset(this, 0, sizeof(VulkanRenderPassFullHashStruct));
    };
    uint8 attachmentCount;
    uint8 sampleCount;
    ImageFormat formats[MaxRenderTargets];
    ImageLayout initialLayout[MaxRenderTargets];
    ImageLayout finalLayout[MaxRenderTargets];
};

struct VulkanRenderTargetInfo
{
    // 仅用于 VulkanPipeline 创建时传入的 RenderPass，提供兼容性，录制命令时并不使用
    VulkanRenderTargetInfo(const RenderTargetDesc &renderPassInfo);

    // 实际 BeginRenderPass 时使用的
    VulkanRenderTargetInfo(const RHIRenderPassInfo &renderPassInfo);

    uint32 compatibleHash;
    uint32 fullHash;
  
    struct ColorAttachmentInfo
    {
        ImageLayout initialLayout;
        ImageLayout finalLayout;
        ImageFormat format;
    };
  
    struct DepthStencilAttachmentInfo
    {
        ImageLayout initialLayout;
        ImageLayout finalLayout;
        ImageFormat format;
    };
  
    std::vector<ColorAttachmentInfo> colorAttachmentInfos;
    std::optional<DepthStencilAttachmentInfo> depthStencilAttachmentInfo;
    uint32 attachmentCount;
    uint32 sampleCount;
};

class VulkanRenderPass
{
  public:
    VulkanRenderPass(const VulkanRenderTargetInfo &renderTargetInfo, VulkanDevice &device);
  
    vk::raii::RenderPass &GetHandle() { return handle; }

  private:
    vk::raii::RenderPass handle;
};

class VulkanTexture;

class VulkanFrameBuffer 
{
  public:
    VulkanFrameBuffer(const RHIRenderPassInfo &renderPassInfo, const VulkanRenderTargetInfo &renderTargetInfo, VulkanRenderPass &renderPass, VulkanDevice &device);
  
    bool Matches(const RHIRenderPassInfo &renderPassInfo);

    bool ContainTexture(VulkanTexture *texture) const;
  
    vk::raii::Framebuffer &GetHandle() { return handle; }
  
    uint32 GetWidth() const { return width; }

    uint32 GetHeight() const { return height; }
    
  private:
    vk::raii::Framebuffer handle;
    std::vector<vk::Image> colorAttachments;
    std::optional<vk::Image> depthStencilAttachment;

    uint32 width;
    uint32 height;
};

class VulkanRenderPassManager
{
  public:
    VulkanRenderPassManager(VulkanDevice &device)
        : device(device) {}

    VulkanFrameBuffer *GetOrCreateFrameBuffer(const VulkanRenderTargetInfo &renderTargetInfo, const RHIRenderPassInfo &renderPassInfo, VulkanRenderPass &renderPass);

    VulkanRenderPass *GetOrCreateRenderPass(const VulkanRenderTargetInfo &renderTargetInfo);

    void OnDestroyTexture(VulkanTexture *texture);
  
  private:
    VulkanDevice &device;

    std::unordered_map<uint32, Scope<VulkanRenderPass>> renderPasses; 
  
    using FrameBufferList = std::vector<Scope<VulkanFrameBuffer>>; // 同个RenderTargetLayout的所有FrameBuffer
    std::unordered_map<uint32, FrameBufferList> frameBuffers;
};


}