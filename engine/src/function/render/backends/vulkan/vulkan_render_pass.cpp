#include "vulkan_render_pass.h"
#include "vulkan_context.h"
#include "vulkan_image.h"

namespace Zafkiel
{

static vk::PipelineStageFlags ImageLayoutToPipelineStage(ImageLayout layout)
{
    switch (layout)
    {
        using enum ImageLayout;
    case ColorAttachment: return vk::PipelineStageFlagBits::eColorAttachmentOutput;
    case DepthAttachment: return vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    case DepthStencilAttachment: return vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    case ShaderReadOnly: return vk::PipelineStageFlagBits::eFragmentShader;
    case PresentSrc: return vk::PipelineStageFlagBits::eBottomOfPipe;
    default:
        Log::Error("Unsupported Image Layout!");
        return {};
    }
}

static vk::AccessFlags ImageLayoutToAccessMask(ImageLayout layout)
{
    switch (layout)
    {
        using enum ImageLayout;
    case ColorAttachment: return vk::AccessFlagBits::eColorAttachmentWrite;
    case DepthAttachment: return vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    case DepthStencilAttachment: return vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    case ShaderReadOnly: return vk::AccessFlagBits::eShaderRead;
    case PresentSrc: return vk::AccessFlagBits::eMemoryRead;
    default:
        Log::Error("Unsupported Image Layout!");
        return {};
    }
}

VulkanRenderPassBackend::VulkanRenderPassBackend(const RenderPassSpecification &spec, const Scope<VulkanDevice> &device)
    : renderPass(nullptr), device(device)
{
    vk::RenderPassCreateInfo createInfo;

    // Attachment Descriptions
    std::vector<vk::AttachmentDescription> attachmentDescs;
    for (size_t i = 0; i < spec.attachments.size(); i++)
    {
        auto &attachment = spec.attachments[i];
        vk::AttachmentDescription attachmentDesc;
        attachmentDesc.setFormat(ImageFormatToVulkanType(attachment.format))
                      .setInitialLayout(ImageLayoutToVulkanType(attachment.initialLayout))
                      .setFinalLayout(ImageLayoutToVulkanType(attachment.finalLayout))
                      .setLoadOp(vk::AttachmentLoadOp::eClear)
                      .setStoreOp(vk::AttachmentStoreOp::eStore) // TODO: 优化
                      .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                      .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                      .setSamples(SampleCountToVulkanType(attachment.samples));
        attachmentDescs.push_back(attachmentDesc);
    }
    createInfo.setAttachments(attachmentDescs);

    // Subpasses
    std::vector<vk::SubpassDescription> subpasses;
    std::vector<std::vector<vk::AttachmentReference>> colorAttachRefs; // 持久化!
    std::vector<vk::AttachmentReference> depthStencilAttachRefs;                                                    
    for (auto &subpussSpec : spec.subpasses)
    {
        vk::SubpassDescription subpass;
        std::vector<vk::AttachmentReference> curSubpassColorRefs;
        bool curSubpassHasDepthStencilAttach = false;
        for (auto &attachRefSpec : subpussSpec.attachmentRefs)
        {
            vk::AttachmentReference attachmentRef;
            if (attachRefSpec.layout == ImageLayout::DepthAttachment || attachRefSpec.layout == ImageLayout::DepthStencilAttachment)
            {
                if (curSubpassHasDepthStencilAttach)
                {
                    Log::Error("Repeated Depth Stencil Attachment!");
                }
                else
                {
                    attachmentRef.setAttachment(attachRefSpec.index)
                                 .setLayout(ImageLayoutToVulkanType(attachRefSpec.layout));
                    depthStencilAttachRefs.push_back(attachmentRef);
                    curSubpassHasDepthStencilAttach = true;
                }
            }
            else
            {
                attachmentRef.setAttachment(attachRefSpec.index)
                             .setLayout(ImageLayoutToVulkanType(attachRefSpec.layout));
                curSubpassColorRefs.push_back(attachmentRef);
            }
        }
        colorAttachRefs.push_back(std::move(curSubpassColorRefs));
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
               .setColorAttachments(colorAttachRefs.back());
        if (curSubpassHasDepthStencilAttach)
        {
            subpass.setPDepthStencilAttachment(&depthStencilAttachRefs.back());
        }
        subpasses.push_back(subpass); 
    }
    createInfo.setSubpasses(subpasses);

    // Subpass Dependencies
    std::vector<vk::SubpassDependency> dependencies;
    uint32_t firstSubpass = 0, lastSubpass = subpasses.size() - 1;

    std::vector<ImageLayout> currentAttachmentLayouts(spec.attachments.size());
    std::vector<int> currentLayoutSubpasses(spec.attachments.size());
    for (size_t i = 0; i < spec.attachments.size(); i++)
    {
        currentAttachmentLayouts[i] = spec.attachments[i].initialLayout;
        currentLayoutSubpasses[i] = -1;
    }

    // External -> 第一个subpass
    for (auto &attachRefSpec : spec.subpasses[firstSubpass].attachmentRefs)
    {
        if (attachRefSpec.layout == currentAttachmentLayouts[attachRefSpec.index]) continue;

        vk::SubpassDependency dependency;
        dependency.setSrcSubpass(vk::SubpassExternal)
                  .setDstSubpass(firstSubpass)
                  .setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe)
                  .setDstStageMask(ImageLayoutToPipelineStage(attachRefSpec.layout))
                  .setSrcAccessMask({})
                  .setDstAccessMask(ImageLayoutToAccessMask(attachRefSpec.layout))
                  .setDependencyFlags(vk::DependencyFlagBits::eByRegion);
        dependencies.push_back(dependency);

        currentAttachmentLayouts[attachRefSpec.index] = attachRefSpec.layout;
        currentLayoutSubpasses[attachRefSpec.index] = 0;
    }

    // subpass之间
    for (size_t i = 1; i < subpasses.size(); i++)
    {
        for (auto &attachRefSpec : spec.subpasses[i].attachmentRefs)
        {
            auto &currentLayout = currentAttachmentLayouts[attachRefSpec.index];
            auto &currentLayoutSubpass = currentLayoutSubpasses[attachRefSpec.index];
            if (attachRefSpec.layout == currentLayout) continue;

            vk::SubpassDependency dependency;
            dependency.setSrcSubpass((currentLayoutSubpass == -1) ? vk::SubpassExternal : currentLayoutSubpass)
                      .setDstSubpass(i)
                      .setSrcStageMask(ImageLayoutToPipelineStage(currentLayout))
                      .setDstStageMask(ImageLayoutToPipelineStage(attachRefSpec.layout))
                      .setSrcAccessMask(ImageLayoutToAccessMask(currentLayout))
                      .setDstAccessMask(ImageLayoutToAccessMask(attachRefSpec.layout))
                      .setDependencyFlags(vk::DependencyFlagBits::eByRegion);
            dependencies.push_back(dependency);

            currentLayout = attachRefSpec.layout;
            currentLayoutSubpass = i;
        }
    }

    // 最后一个subpass -> External
    for (auto &attachRefSpec : spec.subpasses[lastSubpass].attachmentRefs)
    {
        auto &currentLayout = currentAttachmentLayouts[attachRefSpec.index];
        auto &currentLayoutSubpass = currentLayoutSubpasses[attachRefSpec.index];
        if (attachRefSpec.layout == currentLayout) continue;
        vk::SubpassDependency dependency;
        dependency.setSrcSubpass(currentLayoutSubpass)
                  .setDstSubpass(vk::SubpassExternal)
                  .setSrcStageMask(ImageLayoutToPipelineStage(currentLayout))
                  .setDstStageMask(ImageLayoutToPipelineStage(spec.attachments[attachRefSpec.index].finalLayout))
                  .setSrcAccessMask(ImageLayoutToAccessMask(currentLayout))
                  .setDstAccessMask(ImageLayoutToAccessMask(spec.attachments[attachRefSpec.index].finalLayout))
                  .setDependencyFlags(vk::DependencyFlagBits::eByRegion);
        dependencies.push_back(dependency);
    }
    
    createInfo.setDependencies(dependencies);

    renderPass = device->GetHandle().createRenderPass(createInfo);
}

Scope<RenderPass> VulkanRenderPassFactory::Create(const RenderPassSpecification &spec, const Scope<VulkanDevice> &device)
{
    auto backend = CreateScope<VulkanRenderPassBackend>(spec, device);
    return CreateScope<RenderPass>(spec, std::move(backend));
}


}