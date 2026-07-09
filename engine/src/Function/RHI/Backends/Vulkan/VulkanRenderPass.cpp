#include "Function/RHI/Backends/Vulkan/VulkanRenderPass.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanTexture.h"
#include "Core/Base/Crc.h"

namespace Zafkiel 
{

vk::ClearValue ClearValueToVulkanType(ClearValue value, AttachmentType attachType, ImageFormat attachFormat)
{
    vk::ClearValue result;
    if (attachType == AttachmentType::Color)
    {
        switch (attachFormat)
        {
            using enum ImageFormat;
        case R8:
        case R8_sRGB:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.floatValue, 0, 0, 0}));
            break;
        case RG8:
        case RG8_sRGB:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.vec2Value.x, value.vec2Value.y, 0, 0}));
            break;
        case BGR8:
        case RGB8:
        case RGB8_sRGB:
        case BGR8_sRGB:
        case RGB16F:
        case RGB32F:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.vec3Value.x, value.vec3Value.y, value.vec3Value.z, 0}));
            break;
        case BGRA8:
        case RGBA8:
        case RGBA8_sRGB:
        case BGRA8_sRGB:
        case RGBA16F:
        case RGBA32F:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.vec4Value.x, value.vec4Value.y, value.vec4Value.z, value.vec4Value.w}));
            break;
        case R32UI:
            result.setColor(vk::ClearColorValue(std::array<uint32, 4>{value.uintValue, 0, 0, 0}));
            break;
        case R32F:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.floatValue, 0, 0, 0}));
            break;
        default:
            Log::Error("Unsupported Clear Color Value!");
            break;
        }
    }
    else if (attachType == AttachmentType::DepthStencil)
    {
        result.setDepthStencil(vk::ClearDepthStencilValue{value.floatValue, value.uintValue});
    }
    else if (attachType == AttachmentType::Depth)
    {
        result.setDepthStencil(vk::ClearDepthStencilValue{value.floatValue, {}});
    }
    else if (attachType == AttachmentType::Stencil)
    {
        result.setDepthStencil(vk::ClearDepthStencilValue{{}, value.uintValue});
    }
    else
    {
        Log::Error("Unknown ClearValue Format!");
    }
    return result;
}

VulkanRenderTargetInfo::VulkanRenderTargetInfo(const RHIRenderPassInfo &renderPassInfo)
{
    VulkanRenderPassCompatibleHashStruct compatibleHashStruct;
    VulkanRenderPassFullHashStruct fullHashStruct;

    attachmentCount = renderPassInfo.colorAttachments.size()
                    + (renderPassInfo.depthStencilAttachment.has_value() ? 1 : 0);
    compatibleHashStruct.attachmentCount = attachmentCount;
    fullHashStruct.attachmentCount = attachmentCount;

    if (!renderPassInfo.colorAttachments.empty())
        sampleCount = renderPassInfo.colorAttachments[0].texture->GetSampleCount();
    else if (renderPassInfo.depthStencilAttachment.has_value())
        sampleCount = renderPassInfo.depthStencilAttachment.value().texture->GetSampleCount();
    compatibleHashStruct.sampleCount = sampleCount;
    fullHashStruct.sampleCount = sampleCount;

    for (uint32 i = 0; i < renderPassInfo.colorAttachments.size(); i++)
    {
        ColorAttachmentInfo info
        {
            .initialLayout = renderPassInfo.colorAttachments[i].initialLayout,
            .finalLayout = renderPassInfo.colorAttachments[i].finalLayout,
            .format = renderPassInfo.colorAttachments[i].texture->GetFormat(),
        };
        colorAttachmentInfos.push_back(info);

        compatibleHashStruct.formats[i] = renderPassInfo.colorAttachments[i].texture->GetFormat();
        fullHashStruct.formats[i] = renderPassInfo.colorAttachments[i].texture->GetFormat();
        fullHashStruct.initialLayout[i] = renderPassInfo.colorAttachments[i].initialLayout;
        fullHashStruct.finalLayout[i] = renderPassInfo.colorAttachments[i].finalLayout;
    }
    if (renderPassInfo.depthStencilAttachment.has_value())
    {
        DepthStencilAttachmentInfo info
        {
            .initialLayout = renderPassInfo.depthStencilAttachment.value().initialLayout,
            .finalLayout = renderPassInfo.depthStencilAttachment.value().finalLayout,
            .format = renderPassInfo.depthStencilAttachment.value().texture->GetFormat(),
        };
        depthStencilAttachmentInfo = info;

        uint32 index = renderPassInfo.colorAttachments.size();
        compatibleHashStruct.formats[index] = renderPassInfo.depthStencilAttachment.value().texture->GetFormat();
        fullHashStruct.formats[index] = renderPassInfo.depthStencilAttachment.value().texture->GetFormat();
        fullHashStruct.initialLayout[index] = renderPassInfo.depthStencilAttachment.value().initialLayout;
        fullHashStruct.finalLayout[index] = renderPassInfo.depthStencilAttachment.value().finalLayout;
    }
    
    compatibleHash = Crc::MemCrc32(compatibleHashStruct);
    fullHash = Crc::MemCrc32(fullHashStruct);
}

VulkanRenderTargetInfo::VulkanRenderTargetInfo(const RenderTargetDesc &renderTargetDesc)
{
    VulkanRenderPassCompatibleHashStruct compatibleHashStruct;
    VulkanRenderPassFullHashStruct fullHashStruct;

    attachmentCount = renderTargetDesc.colorAttachmentDescs.size()
                    + (renderTargetDesc.depthStencilAttachmentDesc.has_value() ? 1 : 0);
    compatibleHashStruct.attachmentCount = attachmentCount;
    fullHashStruct.attachmentCount = attachmentCount;

    if (!renderTargetDesc.colorAttachmentDescs.empty())
        sampleCount = renderTargetDesc.colorAttachmentDescs[0].sampleCount;
    else if (renderTargetDesc.depthStencilAttachmentDesc.has_value())
        sampleCount = renderTargetDesc.depthStencilAttachmentDesc.value().sampleCount;
    compatibleHashStruct.sampleCount = sampleCount;
    fullHashStruct.sampleCount = sampleCount;

    for (uint32 i = 0; i < renderTargetDesc.colorAttachmentDescs.size(); i++)
    {
        ColorAttachmentInfo info
        {
            .initialLayout = ImageLayout::ColorAttachment,
            .finalLayout = ImageLayout::ColorAttachment,
            .format = renderTargetDesc.colorAttachmentDescs[i].format,
        };
        colorAttachmentInfos.push_back(info);

        compatibleHashStruct.formats[i] = renderTargetDesc.colorAttachmentDescs[i].format;
        fullHashStruct.formats[i] = renderTargetDesc.colorAttachmentDescs[i].format;
    }
    if (renderTargetDesc.depthStencilAttachmentDesc.has_value())
    {
        uint32 index = renderTargetDesc.colorAttachmentDescs.size();

        DepthStencilAttachmentInfo info
        {
            .initialLayout = ImageLayout::DepthStencilAttachment,
            .finalLayout = ImageLayout::DepthStencilAttachment,
            .format = renderTargetDesc.depthStencilAttachmentDesc.value().format,
        };
        depthStencilAttachmentInfo = info;

        compatibleHashStruct.formats[index] = renderTargetDesc.depthStencilAttachmentDesc.value().format;
        fullHashStruct.formats[index] = renderTargetDesc.depthStencilAttachmentDesc.value().format;
    }
    
    compatibleHash = Crc::MemCrc32(compatibleHashStruct);
    fullHash = Crc::MemCrc32(fullHashStruct);
}

VulkanRenderPass::VulkanRenderPass(const VulkanRenderTargetInfo &renderTargetInfo, VulkanDevice &device)
    : handle(nullptr)
{
    vk::RenderPassCreateInfo createInfo;

    // Attachment Descriptions
    std::vector<vk::AttachmentDescription> attachmentDescs;
    for (size_t i = 0; i < renderTargetInfo.colorAttachmentInfos.size(); i++)
    {
        auto attachmentInfo = renderTargetInfo.colorAttachmentInfos[i];

        vk::AttachmentDescription attachmentDesc;
        attachmentDesc.setFormat(ImageFormatToVulkanType(attachmentInfo.format))
                      .setInitialLayout(ImageLayoutToVulkanType(ImageLayout::ColorAttachment))
                      .setFinalLayout(ImageLayoutToVulkanType(ImageLayout::ColorAttachment))
                      .setLoadOp(vk::AttachmentLoadOp::eClear)
                      .setStoreOp(vk::AttachmentStoreOp::eStore)
                      .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                      .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                      .setSamples(SampleCountToVulkanType(renderTargetInfo.sampleCount));
        attachmentDescs.push_back(attachmentDesc);
    }
    if (renderTargetInfo.depthStencilAttachmentInfo.has_value())
    {
        auto &attachmentInfo = renderTargetInfo.depthStencilAttachmentInfo.value();
        auto stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        auto stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        if (attachmentInfo.format == ImageFormat::DEPTH24STENCIL8)
        {
            stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            stencilStoreOp = vk::AttachmentStoreOp::eStore;
        }
        vk::AttachmentDescription attachmentDesc;
        attachmentDesc.setFormat(ImageFormatToVulkanType(attachmentInfo.format))
                      .setInitialLayout(ImageLayoutToVulkanType(ImageLayout::DepthStencilAttachment))
                      .setFinalLayout(ImageLayoutToVulkanType(ImageLayout::DepthStencilAttachment))
                      .setLoadOp(vk::AttachmentLoadOp::eClear)
                      .setStoreOp(vk::AttachmentStoreOp::eStore)
                      .setStencilLoadOp(stencilLoadOp)
                      .setStencilStoreOp(stencilStoreOp)
                      .setSamples(SampleCountToVulkanType(renderTargetInfo.sampleCount));
        attachmentDescs.push_back(attachmentDesc);
    }
    createInfo.setAttachments(attachmentDescs);
               
    // Subpasses
    vk::SubpassDescription subpass;
    vk::AttachmentReference depthStencilAttachRef;                                     
    std::vector<vk::AttachmentReference> colorAttachRefs;

    for (uint32 i = 0; i < renderTargetInfo.colorAttachmentInfos.size(); i++)
    {
        vk::AttachmentReference attachmentRef;
        attachmentRef.setAttachment(i)
                     .setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        colorAttachRefs.push_back(attachmentRef);
    }
    
    if (renderTargetInfo.depthStencilAttachmentInfo.has_value())
    {
        uint32 depthStencilAttachIndex = renderTargetInfo.colorAttachmentInfos.size();
        depthStencilAttachRef.setAttachment(depthStencilAttachIndex)
                             .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics) // TODO: 指定 Pipeline 类型
           .setColorAttachments(colorAttachRefs);

    if (renderTargetInfo.depthStencilAttachmentInfo.has_value())
    {
        subpass.setPDepthStencilAttachment(&depthStencilAttachRef);
    }

    createInfo.setSubpasses(subpass);

    handle = device.GetHandle().createRenderPass(createInfo);

}

VulkanRenderPass* VulkanRenderPassManager::GetOrCreateRenderPass(const VulkanRenderTargetInfo& renderTargetInfo)
{
    if (renderPasses.contains(renderTargetInfo.fullHash))
    {
        return renderPasses[renderTargetInfo.fullHash].get();
    }
    
    renderPasses[renderTargetInfo.fullHash] = CreateScope<VulkanRenderPass>(renderTargetInfo, device);

    return renderPasses[renderTargetInfo.fullHash].get();
}

void VulkanRenderPassManager::OnDestroyTexture(VulkanTexture *texture)
{
    for (auto &[hash, frameBufferList] : frameBuffers)
    {
        frameBufferList.erase(
            std::remove_if(frameBufferList.begin(), frameBufferList.end(), [texture](auto &fb) { return fb->ContainTexture(texture); }),
             frameBufferList.end());
    }
}
    
VulkanFrameBuffer::VulkanFrameBuffer(const RHIRenderPassInfo &renderPassInfo, const VulkanRenderTargetInfo &renderTargetInfo, VulkanRenderPass &renderPass, VulkanDevice &device)
    : handle(nullptr)
{
    std::vector<vk::ImageView> attachmentImageViews;
    for (auto &colorAttachment : renderPassInfo.colorAttachments)
    {
        auto vkTexture = static_cast<VulkanTextureBase *>(colorAttachment.texture);
        auto &image = vkTexture->GetLowLevelImage();
        auto &imageView = vkTexture->GetImageView();
        colorAttachments.push_back(image);
        attachmentImageViews.push_back(*imageView);
    }
    if (renderPassInfo.depthStencilAttachment.has_value())
    {
        auto vkTexture = static_cast<VulkanTextureBase *>(renderPassInfo.depthStencilAttachment.value().texture);
        auto &image = vkTexture->GetLowLevelImage();
        auto &imageView = vkTexture->GetImageView();
        depthStencilAttachment = image;
        attachmentImageViews.push_back(*imageView);
    }

    // TODO: 验证 所有attachment 尺寸相同
    
    if (!renderPassInfo.colorAttachments.empty())
    {
        width = renderPassInfo.colorAttachments[0].texture->GetWidth();
        height = renderPassInfo.colorAttachments[0].texture->GetHeight();
    }
    else if (renderPassInfo.depthStencilAttachment.has_value())
    {
        width = renderPassInfo.depthStencilAttachment.value().texture->GetWidth();
        height = renderPassInfo.depthStencilAttachment.value().texture->GetHeight();
    }
    else 
    {
        Log::Error("FrameBuffer has no attachments!");
        return;
    }

    vk::FramebufferCreateInfo createInfo;
    createInfo.setAttachments(attachmentImageViews)
              .setWidth(width)
              .setHeight(height)
              .setRenderPass(*renderPass.GetHandle())
              .setLayers(1);

    handle = device.GetHandle().createFramebuffer(createInfo);
}

bool VulkanFrameBuffer::Matches(const RHIRenderPassInfo &renderPassInfo)
{
    if (colorAttachments.size() != renderPassInfo.colorAttachments.size())
    {
        return false;
    }
    if (renderPassInfo.depthStencilAttachment.has_value())
    {
        auto vkTexture = static_cast<VulkanTexture *>(renderPassInfo.depthStencilAttachment.value().texture);
        if (*vkTexture->GetImage() != depthStencilAttachment.value())
        {
            return false;
        }
    }
    for (auto [index, colorAttachmentInfo] : std::views::enumerate(renderPassInfo.colorAttachments))
    {
        auto vkTexture = static_cast<VulkanTexture *>(colorAttachmentInfo.texture);
        if (*vkTexture->GetImage() != colorAttachments[index])
        {
            return false;
        }
    }
    return true;
}

bool VulkanFrameBuffer::ContainTexture(VulkanTexture *texture) const
{
    for (auto &colorAttachment : colorAttachments)
    {
        if (*texture->GetImage() == colorAttachment) return true;
    }
    if (depthStencilAttachment.has_value())
    {
        if (*texture->GetImage() == depthStencilAttachment.value()) return true;
    }
    return false;
}

VulkanFrameBuffer *VulkanRenderPassManager::GetOrCreateFrameBuffer(const VulkanRenderTargetInfo &renderTargetInfo, const RHIRenderPassInfo &renderPassInfo, VulkanRenderPass &renderPass)
{
    auto frameBufferHash = renderTargetInfo.compatibleHash;

    auto FindFrameBufferInList = [&](FrameBufferList &list)
    {
        VulkanFrameBuffer *result = nullptr;

        for (auto &frameBuffer : list)
        {
            if (frameBuffer->Matches(renderPassInfo))
            {
                result = frameBuffer.get();
                break;
            }
        }
        return result;
    };
    
    FrameBufferList *frameBufferList;
    if (frameBuffers.contains(frameBufferHash))
    {
        frameBufferList = &frameBuffers[frameBufferHash];
        auto result = FindFrameBufferInList(*frameBufferList);
        if (result)
        {
            return result;
        }
    }
    else 
    {
        frameBuffers[frameBufferHash] = std::vector<Scope<VulkanFrameBuffer>>();
        frameBufferList = &frameBuffers[frameBufferHash];
    }
    
    auto newFrameBuffer = CreateScope<VulkanFrameBuffer>(renderPassInfo, renderTargetInfo, renderPass, device);
    frameBufferList->push_back(MoveTemp(newFrameBuffer));

    return frameBufferList->back().get();
}

}
