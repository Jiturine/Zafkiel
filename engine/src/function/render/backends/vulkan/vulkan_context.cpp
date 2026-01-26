#include "function/render/backends/vulkan/vulkan_context.h"
#include "function/render/backends/vulkan/vulkan_image.h"
#include "function/render/backends/vulkan/vulkan_shader_module.h"
#include "function/render/backends/vulkan/vulkan_texture.h"
#include "function/render/backends/vulkan/vulkan_render_pass.h"
#include "function/render/backends/vulkan/vulkan_vertex_buffer.h"
#include "function/render/backends/vulkan/vulkan_index_buffer.h"
#include "function/render/backends/vulkan/vulkan_shader.h"
#include "function/render/backends/vulkan/vulkan_shader_material_template.h"
#include "function/render/backends/vulkan/vulkan_shader_material.h"
#include "function/render/backends/vulkan/vulkan_object_shader_material.h"


#include <SDL3/SDL_vulkan.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

namespace Zafkiel
{

static constexpr uint32_t GlobalShaderMaterialSet = 0;
static constexpr uint32_t PassMaterialSet = 1;
static constexpr uint32_t MaterialSet = 2;
static constexpr uint32_t ObjectShaderMaterialSet = 3;

VulkanContext::VulkanContext(const Window &window)
    : instance(nullptr), surface(nullptr), window(window.GetHandle())
{
    // 创建 vk::Instance
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_API_VERSION_1_4);

    createInfo.setPApplicationInfo(&appInfo);

    uint32_t extensionCount;
    const char *const *extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    for (size_t i = 0; i < extensionCount; i++)
        this->extensions.push_back(extensions[i]);
    this->extensions.push_back(vk::EXTSurfaceMaintenance1ExtensionName);     // 
    this->extensions.push_back(vk::KHRGetSurfaceCapabilities2ExtensionName); // swapchain 同步扩展 

    std::vector<const char *> layers = {"VK_LAYER_KHRONOS_validation"};
    createInfo.setPpEnabledLayerNames(layers.data())
              .setEnabledLayerCount(layers.size())
              .setEnabledExtensionCount(this->extensions.size())
              .setPpEnabledExtensionNames(this->extensions.data());

    instance = context.createInstance(createInfo);

    // 从SDL获取surface
    VkSurfaceKHR cStyleSurface;
    bool success = SDL_Vulkan_CreateSurface(window.GetHandle(), *instance, nullptr, &cStyleSurface);
    if (!success)
    {
        Log::Error("Error when Create Surface: {}", SDL_GetError());
    }
    surface = vk::raii::SurfaceKHR { instance, cStyleSurface };

    // 选择物理设备
    auto devices = instance.enumeratePhysicalDevices();
    physicalDevice = CreateScope<VulkanPhysicalDevice>(devices[0], surface);

    // 创建逻辑设备
    device = CreateScope<VulkanDevice>(Borrow(physicalDevice));

    // 创建描述符池
    descriptorManager = CreateScope<VulkanDescriptorManager>(Borrow(device));

    // 创建交换链所需的主RenderPass
    RenderPassSpecification spec
    {
        .attachments =
        {
            {
                .format = ImageFormat::RGBA8,
                .initialLayout = ImageLayout::Undefined,
                .finalLayout = ImageLayout::PresentSrc,
                .samples = 1
            }
        },
        .subpasses =
        {
            {
                .attachmentRefs = {
                    {0, ImageLayout::ColorAttachment}
                },
                .type = PipelineType::Graphics
            }
        }
    };
    mainRenderPass = CreateScope<RenderPass>(spec, CreateRenderPassBackend(spec));

    auto capabilities = physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface);
    isSurfaceSizeFixed = capabilities.minImageExtent.width == capabilities.maxImageExtent.width
                           && capabilities.minImageExtent.height == capabilities.maxImageExtent.height;
    // 创建交换链
    VulkanSwapchainSpecification swapchainSpecification
    {
        .device = Borrow(device),
        .physicalDevice = Borrow(physicalDevice),
        .renderPass = Borrow(mainRenderPass),
        .surface = surface,
        .width = window.GetWidth(),
        .height = window.GetHeight()
    };
    swapchain = CreateScope<VulkanSwapchain>(swapchainSpecification);

    // 创建命令池
    commandManager = CreateScope<VulkanCommandManager>(Borrow(device), Borrow(physicalDevice));

    // 创建flight
    for (size_t i = 0; i < flightCount; i++)
    {
        framesInFlight.emplace_back(CreateScope<VulkanFrameInFlight>(Borrow(commandManager), Borrow(device)));
    }
    curFrame = 0;

    device->GetHandle().waitIdle();
}

void VulkanContext::Resize(uint32_t width, uint32_t height)
{
    ResizeSwapchain(width, height);
}

Scope<PipelineBackend> VulkanContext::CreatePipelineBackend(const GraphicsPipelineSpecification &spec, Borrow<GraphicsShader> shader, Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule, Borrow<RenderPass> renderPass, Borrow<RenderRegistryView> registryView)
{
    vk::GraphicsPipelineCreateInfo createInfo;

    // 顶点输入阶段
    vk::PipelineVertexInputStateCreateInfo vertexInputState;

    auto &vertexBufferLayout = shader->GetReflection().vertexInput;
    std::vector<vk::VertexInputAttributeDescription> attributes(vertexBufferLayout.elements.size());
    uint32_t stride = 0;
    for (size_t i = 0; i < attributes.size(); i++)
    {
        auto &element = vertexBufferLayout.elements[i];
        attributes[i].setBinding(0)
                     .setFormat(ShaderDataTypeToVulkanFormat(element.type))
                     .setLocation(i)
                     .setOffset(element.offset);
    }
    vk::VertexInputBindingDescription binding;
    binding.setBinding(0)
           .setInputRate(vk::VertexInputRate::eVertex)
           .setStride(vertexBufferLayout.stride);

    vertexInputState.setVertexAttributeDescriptions(attributes);
    vertexInputState.setVertexBindingDescriptions(binding);

    createInfo.setPVertexInputState(&vertexInputState);

    // 顶点组装阶段
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.setPrimitiveRestartEnable(false)
                 .setTopology(PrimitiveTopologyToVulkanType(spec.primitiveTopology));
    createInfo.setPInputAssemblyState(&inputAssembly);

    // 配置着色器
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(2);
    shaderStages[0].setStage(vk::ShaderStageFlagBits::eVertex)
                   .setModule(vertexModule->GetBackend().As<VulkanShaderModuleBackend>()->GetHandle())
                   .setPName("main");
    shaderStages[1].setStage(vk::ShaderStageFlagBits::eFragment)
                   .setModule(fragmentModule->GetBackend().As<VulkanShaderModuleBackend>()->GetHandle())
                   .setPName("main");
    createInfo.setStages(shaderStages);

    // 配置视口 (动态视口)
    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.setViewportCount(1)
                 .setScissorCount(1);
    createInfo.setPViewportState(&viewportState);

    vk::PipelineDynamicStateCreateInfo dynamicState;
    std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    dynamicState.setDynamicStates(dynamicStates);
    createInfo.setPDynamicState(&dynamicState);

    // 光栅化阶段
    vk::PipelineRasterizationStateCreateInfo rast;
    rast.setRasterizerDiscardEnable(false)
        .setCullMode(CullModeToVulkanType(spec.cullMode))
        .setFrontFace(FrontFaceToVulkanType(spec.frontFace))
        .setPolygonMode(PolygonModeToVulkanType(spec.polygonMode))
        .setLineWidth(1);
    createInfo.setPRasterizationState(&rast);

    // 配置多重采样
    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.setSampleShadingEnable(false)
               .setRasterizationSamples(vk::SampleCountFlagBits::e1);
    createInfo.setPMultisampleState(&multisample);

    // 融混阶段 TODO: 优化
    vk::PipelineColorBlendStateCreateInfo blend;
    std::vector<vk::PipelineColorBlendAttachmentState> attachments(spec.colorAttachmentCount);
    for (size_t i = 0; i < spec.colorAttachmentCount; i++)
    {
        attachments[i].setBlendEnable(false)
                      .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    }
    blend.setLogicOpEnable(false)
         .setAttachments(attachments);
    createInfo.setPColorBlendState(&blend);

    // 深度测试
    vk::PipelineDepthStencilStateCreateInfo depthStencil;
    depthStencil.setDepthTestEnable(spec.depthTest)
                .setDepthWriteEnable(true)
                .setDepthCompareOp(vk::CompareOp::eLess)
                .setDepthBoundsTestEnable(false); // TODO: 设置
                // .setMinDepthBounds(0.0f)
                // .setMaxDepthBounds(1.0f);
    createInfo.setPDepthStencilState(&depthStencil);

    // 设置layout
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    std::vector<vk::DescriptorSetLayout> setLayouts;
    for (auto &templateHandle : spec.shaderMaterialTemplates)
    {
        if (registryView->IsShaderMaterialTemplate(templateHandle))
        {
            auto shaderMaterialTemplate = registryView->GetShaderMaterialTemplate(templateHandle);
            setLayouts.push_back(*shaderMaterialTemplate->GetBackend().As<VulkanShaderMaterialTemplateBackend>()->GetDescriptorSetLayout());
        }
        else if (registryView->IsObjectShaderMaterialTemplate(templateHandle))
        {
            auto objectShaderMaterialTemplate = registryView->GetObjectShaderMaterialTemplate(templateHandle);
            setLayouts.push_back(*objectShaderMaterialTemplate->GetBackend().As<VulkanObjectShaderMaterialTemplateBackend>()->GetDescriptorSetLayout());
        }
        else
        {
            setLayouts.push_back({});
        }
    }
    pipelineLayoutCreateInfo.setSetLayouts(setLayouts);
    auto pipelineLayout = device->GetHandle().createPipelineLayout(pipelineLayoutCreateInfo);

    createInfo.setRenderPass(renderPass->GetBackend().As<VulkanRenderPassBackend>()->GetHandle())
              .setSubpass(0)
              .setLayout(pipelineLayout);

    auto pipeline = device->GetHandle().createGraphicsPipeline(nullptr, createInfo);
    return CreateScope<VulkanPipelineBackend>(std::move(pipeline), std::move(pipelineLayout));
}

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
Scope<RenderPassBackend> VulkanContext::CreateRenderPassBackend(const RenderPassSpecification &spec)
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

    auto renderPass = device->GetHandle().createRenderPass(createInfo);

    return CreateScope<VulkanRenderPassBackend>(std::move(renderPass));
}

Scope<ShaderBackend> VulkanContext::CreateShaderBackend(Borrow<VertexModule> vertexModule, Borrow<FragmentModule> fragmentModule)
{
    // Vulkan 的 ShaderBackend 不需要 vertex/fragment module 对象
    return CreateScope<VulkanShaderBackend>();
}

Scope<ShaderModuleBackend> VulkanContext::CreateShaderModuleBackend(Buffer buffer, ShaderStage stage)
{
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setCode({buffer.Size<uint32_t>(), buffer.Data<uint32_t>()});
    auto shaderModule = device->GetHandle().createShaderModule(createInfo);
    return CreateScope<VulkanShaderModuleBackend>(std::move(shaderModule));
}

Scope<ShaderMaterialBackend> VulkanContext::CreateShaderMaterialBackend(Borrow<ShaderMaterialTemplateBackend> shaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema)
{
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.setDescriptorPool(*descriptorManager->GetDescriptorPool())
             .setDescriptorSetCount(flightCount);

    std::vector<vk::DescriptorSetLayout> setLayouts(flightCount, *shaderMaterialTemplateBackend.As<VulkanShaderMaterialTemplateBackend>()->GetDescriptorSetLayout());
    allocInfo.setSetLayouts(setLayouts);

    auto descriptorSets = device->GetHandle().allocateDescriptorSets(allocInfo);
    std::vector<std::optional<std::vector<Scope<UniformBuffer>>>> uniformBuffers(schema->GetResourceTypeInfos().size());

    std::vector<vk::WriteDescriptorSet> writeInfos;

    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schema->GetResourceTypeInfos()))
    {
        if (!resourceTypeInfo) continue;
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            uint32_t size = resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>()->GetSize();
            uniformBuffers[binding] = std::vector<Scope<UniformBuffer>>();
            for (size_t i = 0; i < flightCount; i++)
            {
                auto uniformBufferBackend = CreateUniformBufferBackend(size);
                uniformBuffers[binding].value().push_back(CreateScope<UniformBuffer>(size, std::move(uniformBufferBackend)));
                auto uniformBuffer = Borrow(uniformBuffers[binding].value().back());

                vk::DescriptorBufferInfo bufferInfo;
                bufferInfo.setBuffer(*uniformBuffer->GetBackend().As<VulkanUniformBufferBackend>()->GetHandle())
                        .setOffset(0)
                        .setRange(uniformBuffer->GetSize());
                vk::WriteDescriptorSet writeInfo;
                writeInfo.setDescriptorCount(1)
                        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                        .setBufferInfo(bufferInfo)
                        .setDstBinding(binding)
                        .setDstSet(descriptorSets[i]);
                writeInfos.push_back(writeInfo);
            }
        }
    }
    device->GetHandle().updateDescriptorSets(writeInfos, {});
    
    return CreateScope<VulkanShaderMaterialBackend>(flightCount, std::move(descriptorSets), std::move(uniformBuffers));
}

Scope<ShaderMaterialTemplateBackend> VulkanContext::CreateShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema)
{
    auto &paramTypeInfos = schema->GetResourceTypeInfos();
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    for (auto [binding, resourceTypeInfo] : std::views::enumerate(paramTypeInfos))
    {
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            vk::DescriptorSetLayoutBinding layoutBinding;
            layoutBinding.setBinding(binding)
                         .setDescriptorCount(1)
                         .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                         .setStageFlags(vk::ShaderStageFlagBits::eAll);
            bindings.push_back(layoutBinding);
        }
        else if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            vk::DescriptorSetLayoutBinding layoutBinding;
            layoutBinding.setBinding(binding)
                         .setDescriptorCount(1)
                         .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                         .setStageFlags(vk::ShaderStageFlagBits::eAll);
            bindings.push_back(layoutBinding);
        }
    }

    vk::DescriptorSetLayoutCreateInfo setLayoutCreateInfo;
    setLayoutCreateInfo.setBindings(bindings);

    auto descriptorSetLayout = device->GetHandle().createDescriptorSetLayout(setLayoutCreateInfo);
    
    return CreateScope<VulkanShaderMaterialTemplateBackend>(std::move(descriptorSetLayout));
}

Scope<VertexBufferBackend> VulkanContext::CreateVertexBufferBackend(const float *vertices, uint32_t size)
{
    VulkanBufferSpecification stagingBufferSpec
    {
        .size = size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .property = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };
    auto stagingBuffer = CreateScope<VulkanBuffer>(stagingBufferSpec, Borrow(device), Borrow(physicalDevice));
    VulkanBufferSpecification deviceBufferSpec
    {
        .size = size,
        .usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        .property = vk::MemoryPropertyFlagBits::eDeviceLocal
    };
    auto buffer = CreateScope<VulkanBuffer>(deviceBufferSpec, Borrow(device), Borrow(physicalDevice));

    void *ptr = stagingBuffer->memory.mapMemory(0, stagingBuffer->size);
    memcpy(ptr, vertices, size);
    stagingBuffer->memory.unmapMemory();

    // 共享内存传输到GPU DeviceLocal
    auto cmdBuf = commandManager->CreateOneCommandBuffer();
    vk::CommandBufferBeginInfo begin;
    begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf.begin(begin);
    {
        vk::BufferCopy region;
        region.setSize(size)
              .setSrcOffset(0)
              .setDstOffset(0);
        cmdBuf.copyBuffer(stagingBuffer->buffer, buffer->buffer, region);
    }
    cmdBuf.end();

    vk::SubmitInfo submit;
    submit.setCommandBuffers(*cmdBuf);
    device->GetGraphicsQueue().submit(submit);

    device->GetHandle().waitIdle();

    return CreateScope<VulkanVertexBufferBackend>(std::move(buffer));
}
Scope<IndexBufferBackend> VulkanContext::CreateIndexBufferBackend(const uint32_t *indices, uint32_t count)
{
    uint32_t size = (uint32_t)sizeof(uint32_t) * count;
    VulkanBufferSpecification stagingBufferSpec
    {
        .size = size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .property = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };
    auto stagingBuffer = CreateScope<VulkanBuffer>(stagingBufferSpec, Borrow(device), Borrow(physicalDevice));
    VulkanBufferSpecification deviceBufferSpec
    {
        .size = size,
        .usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        .property = vk::MemoryPropertyFlagBits::eDeviceLocal
    };
    auto buffer = CreateScope<VulkanBuffer>(deviceBufferSpec, Borrow(device), Borrow(physicalDevice));

    void *ptr = stagingBuffer->memory.mapMemory(0, stagingBuffer->size);
    memcpy(ptr, indices, size);
    stagingBuffer->memory.unmapMemory();

    // 共享内存传输到GPU DeviceLocal
    auto cmdBuf = commandManager->CreateOneCommandBuffer();
    vk::CommandBufferBeginInfo begin;
    begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf.begin(begin);
    {
        vk::BufferCopy region;
        region.setSize(size)
              .setSrcOffset(0)
              .setDstOffset(0);
        cmdBuf.copyBuffer(stagingBuffer->buffer, buffer->buffer, region);
    }
    cmdBuf.end();

    vk::SubmitInfo submit;
    submit.setCommandBuffers(*cmdBuf);
    device->GetGraphicsQueue().submit(submit);

    device->GetHandle().waitIdle();

    return CreateScope<VulkanIndexBufferBackend>(std::move(buffer));
}
Scope<UniformBufferBackend> VulkanContext::CreateUniformBufferBackend(uint32_t size) 
{
    VulkanBufferSpecification bufferSpec
    {
        .size = size,
        .usage = vk::BufferUsageFlagBits::eUniformBuffer,
        .property = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };
    auto buffer = CreateScope<VulkanBuffer>(bufferSpec, Borrow(device), Borrow(physicalDevice));
    
    return CreateScope<VulkanUniformBufferBackend>(std::move(buffer));
}
Scope<FrameBufferBackend> VulkanContext::CreateFrameBufferBackend(const FrameBufferSpecification &spec, Borrow<RenderPass> renderPass, const std::vector<Borrow<Image>> &images)
{
    auto renderPassBackend = renderPass->GetBackend().As<VulkanRenderPassBackend>();

    std::vector<vk::raii::Framebuffer> frameBuffers;
    auto imageCount = images[0]->GetBackend().As<VulkanImageBackend>()->GetImageCount();
    for (size_t i = 0; i < imageCount; i++)
    {
        std::vector<vk::ImageView> attachmentImageViews;
        for (size_t j = 0; j < images.size(); j++)
        {
            auto &imageView = images[j]->GetBackend().As<VulkanImageBackend>()->GetImageView(i);
            attachmentImageViews.push_back(*imageView);
        }
        vk::FramebufferCreateInfo createInfo;
        createInfo.setAttachments(attachmentImageViews)
                  .setWidth(spec.width)
                  .setHeight(spec.height)
                  .setRenderPass(*renderPassBackend->GetHandle())
                  .setLayers(1);
        frameBuffers.push_back(device->GetHandle().createFramebuffer(createInfo));
    }

    return CreateScope<VulkanFrameBufferBackend>(std::move(frameBuffers));
}
Scope<ImageBackend> VulkanContext::CreateImageBackend(const ImageSpecification &spec)
{
    uint32_t imageCount = 1;
    if (spec.updateFrequency == ImageUpdateFrequency::Transient)
    {
        imageCount = flightCount;
    }
    vk::ImageUsageFlags usages;
    for (auto imageUsage : spec.usages)
    {
        usages |= ImageUsageToVulkanType(imageUsage);
    }

    device->GetHandle().waitIdle();
    std::vector<vk::raii::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
    std::vector<vk::raii::DeviceMemory> memories;
    // 创建 vk::Image
    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.setImageType(vk::ImageType::e2D)
                   .setArrayLayers(1)
                   .setMipLevels(1)
                   .setExtent({spec.width, spec.height, 1})
                   .setFormat(ImageFormatToVulkanType(spec.format))
                   .setTiling(vk::ImageTiling::eOptimal)
                   .setInitialLayout(vk::ImageLayout::eUndefined)
                   .setUsage(usages)
                   .setSamples(SampleCountToVulkanType(spec.samples));
    for (size_t i = 0; i < imageCount; i++)
    {
        images.emplace_back(device->GetHandle().createImage(imageCreateInfo));
    }

    // 查询分配内存信息
    auto requirements = images[0].getMemoryRequirements();

    uint32_t index = 0;
    auto properties = physicalDevice->GetHandle().getMemoryProperties();
    for (size_t i = 0; i < properties.memoryTypeCount; i++)
    {
        if ((1 << i) & requirements.memoryTypeBits && properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
        {
            index = i;
            break;
        }
    }

    // 分配内存
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.setMemoryTypeIndex(index)
             .setAllocationSize(requirements.size);
    for (size_t i = 0; i < imageCount; i++)
    {
        memories.emplace_back(device->GetHandle().allocateMemory(allocInfo));
    }

    // 绑定内存
    for (size_t i = 0; i < imageCount; i++)
    {
        images[i].bindMemory(memories[i],  0);
    }

    // TODO: 转换内存布局

    // 创建 ImageView

    for (size_t i = 0; i < imageCount; i++)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo;
        vk::ComponentMapping mapping;
        vk::ImageSubresourceRange range;
        range.setBaseMipLevel(0)
             .setLevelCount(1)
             .setBaseArrayLayer(0)
             .setLayerCount(1)
             .setAspectMask(ImageFormatToVulkanImageAspect(spec.format));
        imageViewCreateInfo.setImage(images[i])
                           .setViewType(vk::ImageViewType::e2D)
                           .setComponents(mapping)
                           .setFormat(ImageFormatToVulkanType(spec.format))
                           .setSubresourceRange(range);
        imageViews.emplace_back(device->GetHandle().createImageView(imageViewCreateInfo));
    }
    
    return CreateScope<VulkanImageBackend>(imageCount, std::move(images), std::move(imageViews), std::move(memories));
}

Scope<Texture2DBackend> VulkanContext::CreateTexture2DBackend(const Texture2DSpecification &spec, Borrow<ImageBackend> imageBackend)
{
    vk::SamplerCreateInfo createInfo;
    createInfo.setMagFilter(FilterTypeToVulkanType(spec.filter))
        .setMinFilter(FilterTypeToVulkanType(spec.filter))
        .setAddressModeU(TextureWrapToVulkanAddressMode(spec.wrap))
        .setAddressModeV(TextureWrapToVulkanAddressMode(spec.wrap))
        .setAddressModeW(TextureWrapToVulkanAddressMode(spec.wrap))
        .setAnisotropyEnable(false)
        .setBorderColor(vk::BorderColor::eFloatOpaqueBlack)
        .setUnnormalizedCoordinates(false)
        .setCompareEnable(false)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear);
    auto sampler = device->GetHandle().createSampler(createInfo);

    return CreateScope<VulkanTexture2DBackend>(std::move(sampler));
}

constexpr size_t AlignUp(size_t value, size_t alignment) 
{
    return (value + alignment - 1) & ~(alignment - 1);
}
Scope<ObjectShaderMaterialBackend> VulkanContext::CreateObjectShaderMaterialBackend(Borrow<ObjectShaderMaterialTemplateBackend> objectShaderMaterialTemplateBackend, Borrow<ShaderMaterialSchema> schema)
{
    uint32_t minUniformBufferOffsetAlignment = physicalDevice->GetMinUniformBufferOffsetAlignment();

    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.setDescriptorPool(*descriptorManager->GetDescriptorPool())
             .setDescriptorSetCount(flightCount);

    std::vector<vk::DescriptorSetLayout> setLayouts(flightCount, *objectShaderMaterialTemplateBackend.As<VulkanObjectShaderMaterialTemplateBackend>()->GetDescriptorSetLayout());
    allocInfo.setSetLayouts(setLayouts);

    auto descriptorSets = device->GetHandle().allocateDescriptorSets(allocInfo);
    std::vector<std::optional<std::vector<Scope<UniformBuffer>>>> uniformBuffers(schema->GetResourceTypeInfos().size());

    std::vector<vk::WriteDescriptorSet> writeInfos;

    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schema->GetResourceTypeInfos()))
    {
        if (!resourceTypeInfo) continue;
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            uniformBuffers[binding] = std::vector<Scope<UniformBuffer>>();
            auto uniformBufferType = resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>();
            uint32_t perObjectsize = uniformBufferType->GetSize();
            uint32_t stride = AlignUp(perObjectsize, minUniformBufferOffsetAlignment);
            uint32_t bufferSize = stride * maxObjectNum;
            for (size_t i = 0; i < flightCount; i++)
            {
                auto uniformBufferBackend = CreateUniformBufferBackend(bufferSize);
                uniformBuffers[binding].value().push_back(CreateScope<UniformBuffer>(bufferSize, std::move(uniformBufferBackend)));
                auto uniformBuffer = Borrow(uniformBuffers[binding].value().back());

                vk::DescriptorBufferInfo bufferInfo;
                bufferInfo.setBuffer(*uniformBuffer->GetBackend().As<VulkanUniformBufferBackend>()->GetHandle())
                        .setOffset(0)
                        .setRange(uniformBufferType->GetSize());
                vk::WriteDescriptorSet writeInfo;
                writeInfo.setDescriptorCount(1)
                        .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
                        .setBufferInfo(bufferInfo)
                        .setDstBinding(binding)
                        .setDstSet(descriptorSets[i]);
                writeInfos.push_back(writeInfo);
            }
        }
    }
    device->GetHandle().updateDescriptorSets(writeInfos, {});

    return CreateScope<VulkanObjectShaderMaterialBackend>(minUniformBufferOffsetAlignment, std::move(descriptorSets), std::move(uniformBuffers));
}

Scope<ObjectShaderMaterialTemplateBackend> VulkanContext::CreateObjectShaderMaterialTemplateBackend(Borrow<ShaderMaterialSchema> schema)
{
    auto &paramTypeInfos = schema->GetResourceTypeInfos();
    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
    for (auto [binding, resourceTypeInfo] : std::views::enumerate(paramTypeInfos))
    {
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            vk::DescriptorSetLayoutBinding layoutBinding;
            layoutBinding.setBinding(binding)
                   .setDescriptorCount(1)
                   .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
                   .setStageFlags(vk::ShaderStageFlagBits::eAll);
            layoutBindings.push_back(layoutBinding);
        }
    }

    vk::DescriptorSetLayoutCreateInfo setLayoutCreateInfo;
    setLayoutCreateInfo.setBindings(layoutBindings);

    auto descriptorSetLayout = device->GetHandle().createDescriptorSetLayout(setLayoutCreateInfo);

    return CreateScope<VulkanObjectShaderMaterialTemplateBackend>(std::move(descriptorSetLayout));
}

void VulkanContext::UpdateShaderMaterialDescriptorSet(Borrow<VulkanShaderMaterialBackend> shaderMaterialBackend, const std::vector<std::optional<ShaderResource>> &resources, Borrow<RenderRegistryView> registryView)
{
    std::vector<vk::WriteDescriptorSet> writeInfos;
    std::vector<vk::DescriptorImageInfo> imageInfos;
    imageInfos.reserve(resources.size()); // !!!预留足够大小，确保其不会因扩容改变地址，导致setImageInfo失效
    for (auto [binding, resource] : std::views::enumerate(resources))
    {
        if (!resource) continue;
        if (resource.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage 
            && shaderMaterialBackend->IsDirty(curFrame, binding))
        {
            auto texture = registryView->GetTexture2D(resource.value().handle.value());
            auto image = registryView->GetImage(texture->GetImage());
            vk::DescriptorImageInfo imageInfo;
            imageInfo.setImageView(*image->GetBackend().As<VulkanImageBackend>()->GetImageView(curFrame))
                     .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                     .setSampler(*texture->GetBackend().As<VulkanTexture2DBackend>()->GetSampler());
            imageInfos.push_back(imageInfo);
            vk::WriteDescriptorSet writeInfo;
            writeInfo.setDescriptorCount(1)
                     .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                     .setImageInfo(imageInfos.back())
                     .setDstBinding(binding)
                     .setDstSet(shaderMaterialBackend->GetDescriptorSet(curFrame));
            writeInfos.push_back(writeInfo);

            shaderMaterialBackend->UnsetDirty(curFrame, binding);
        }
    }
    if (!writeInfos.empty()) device->GetHandle().updateDescriptorSets(writeInfos, {});
}

void VulkanContext::UploadShaderMaterialUniform(Borrow<VulkanShaderMaterialBackend> shaderMaterialBackend, const std::vector<std::optional<ShaderResource>> &resources)
{
    for (auto [binding, resource] : std::views::enumerate(resources))
    {
        if (!resource) continue;
        if (resource.value().type->GetCategory() != ShaderReflection::ResourceTypeCategory::UniformBlock) continue;

        Buffer uploadData = resources[binding].value().uniformBuffer;
        auto uniformBufferBackend = shaderMaterialBackend->GetUniformBuffer(curFrame, binding)->GetBackend().As<VulkanUniformBufferBackend>();
        auto src = resource.value().uniformBuffer.Data<uint8_t>();
        auto size = resource.value().uniformBuffer.Size<uint8_t>();
        uniformBufferBackend->SetData(0, size, src);
    }
}

void VulkanContext::UploadObjectShaderMaterialUniform(Borrow<ObjectShaderMaterialBackend> objectShaderMaterialBackend, const std::vector<std::optional<ObjectShaderResource>> &resources)
{
    auto backend = objectShaderMaterialBackend.As<VulkanObjectShaderMaterialBackend>();
    for (auto [binding, resource] : std::views::enumerate(resources))
    {
        if (!resource) continue;
        if (resource.value().type->GetCategory() != ShaderReflection::ResourceTypeCategory::UniformBlock) continue;

        Buffer uploadData = resources[binding].value().dynamicUniformBuffer;
        auto uniformBufferBackend = backend->GetUniformBuffer(curFrame, binding)->GetBackend().As<VulkanUniformBufferBackend>();
        auto uniformBufferType = resource.value().type->As<ShaderReflection::UniformBlock>();
        uint32_t perObjectsize = uniformBufferType->GetSize();
        uint32_t stride = AlignUp(perObjectsize, backend->GetMinUniformBufferOffsetAlignment());
        for (uint32_t i = 0; i < maxObjectNum; i++)
        {
            auto src = resource.value().dynamicUniformBuffer.Data<uint8_t>() + i * perObjectsize;
            uniformBufferBackend->SetData(i * stride, perObjectsize, src);
        }
    }
}

void VulkanContext::ExecuteCmd(std::function<void(vk::raii::CommandBuffer&)> func)
{
    auto cmdBuf = commandManager->CreateOneCommandBuffer();
    vk::CommandBufferBeginInfo begin;
    begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf.begin(begin);
    {
        func(cmdBuf);
    }
    cmdBuf.end();

    vk::SubmitInfo submit;
    submit.setCommandBuffers(*cmdBuf);
    device->GetGraphicsQueue().submit(submit);
    device->GetGraphicsQueue().waitIdle();
}

void VulkanContext::SetImageData(Borrow<Image> image, Buffer buffer)
{
    auto imageBackend = image->GetBackend().As<VulkanImageBackend>();
    // 转换到 TransferDst Layout
    ExecuteCmd([&](vk::raii::CommandBuffer &cmdBuf){
        vk::ImageMemoryBarrier barrier;
        vk::ImageSubresourceRange range;
        range.setLayerCount(1)
             .setBaseArrayLayer(0)
             .setLevelCount(1)
             .setBaseMipLevel(0)
             .setAspectMask(vk::ImageAspectFlagBits::eColor);
        barrier.setImage(imageBackend->GetImage(0))
               .setOldLayout(vk::ImageLayout::eUndefined)
               .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
               .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
               .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
               .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
               .setSubresourceRange(range);
        cmdBuf.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {}, nullptr, barrier);
    });

    // 传输数据
    VulkanBufferSpecification bufferSpec 
    {
        .size = buffer.Size<uint8_t>(),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .property = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    };
    auto vkBuffer = CreateScope<VulkanBuffer>(bufferSpec, Borrow(device), Borrow(physicalDevice));
    void *map = vkBuffer->memory.mapMemory(0, buffer.Size<uint8_t>());
    memcpy(map, buffer.Data<uint8_t>(), buffer.Size<uint8_t>());
    vkBuffer->memory.unmapMemory();

    ExecuteCmd([&](vk::raii::CommandBuffer &cmdBuf){
        vk::BufferImageCopy region;
        vk::ImageSubresourceLayers subsource;
        subsource.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setMipLevel(0)
            .setLayerCount(1);
        region.setBufferImageHeight(0)
            .setBufferOffset(0)
            .setImageOffset(0)
            .setImageExtent({image->GetWidth(), image->GetHeight(), 1})
            .setBufferRowLength(0)
            .setImageSubresource(subsource);
        cmdBuf.copyBufferToImage(vkBuffer->buffer, imageBackend->GetImage(0), vk::ImageLayout::eTransferDstOptimal, region);
    });

    // 转换到Shader ReadOnly Layout
    ExecuteCmd([&](vk::raii::CommandBuffer &cmdBuf){
        vk::ImageMemoryBarrier barrier;
        vk::ImageSubresourceRange range;
        range.setLayerCount(1)
            .setBaseArrayLayer(0)
            .setLevelCount(1)
            .setBaseMipLevel(0)
            .setAspectMask(vk::ImageAspectFlagBits::eColor);
        barrier.setImage(imageBackend->GetImage(0))
            .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
            .setSubresourceRange(range);
        cmdBuf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, nullptr, barrier);
    });
}

std::vector<vk::DescriptorSet> VulkanContext::RegisterImGuiTexture2D(Borrow<VulkanImageBackend> imageBackend, Borrow<VulkanTexture2DBackend> texture2DBackend)
{
    device->GetHandle().waitIdle();
    auto key = *imageBackend->GetImage(0);

    std::vector<vk::DescriptorSet> result;

    // 确保所有图像副本都处于正确的布局
    size_t imageCount = imageBackend->GetImageCount();
    for (size_t i = 0; i < imageCount; i++)
    {
        // 检查并转换图像布局到ShaderReadOnly
        auto cmdBuf = commandManager->CreateOneCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo);

        vk::ImageMemoryBarrier barrier;
        barrier.setOldLayout(vk::ImageLayout::eUndefined)
              .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
              .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
              .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
              .setImage(*imageBackend->GetImage(i))
              .setSubresourceRange({
                  vk::ImageAspectFlagBits::eColor,
                  0, 1, 0, 1
              })
              .setSrcAccessMask(vk::AccessFlagBits::eNone)
              .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        cmdBuf.pipelineBarrier(
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlags{},
            {}, {}, barrier
        );

        cmdBuf.end();

        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(*cmdBuf);
        device->GetGraphicsQueue().submit(submitInfo);
        device->GetHandle().waitIdle();
    }

    for (auto &imageView : imageBackend->GetImageViews())
    {
        auto descriptorSet = ImGui_ImplVulkan_AddTexture(*texture2DBackend->GetSampler(), *imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        result.push_back(descriptorSet);
    }

    registeredImGuiTextures[key] = result;
    return result;
}

void VulkanContext::UnregisterImGuiTexture2D(Borrow<VulkanImageBackend> imageBackend)
{
    device->GetHandle().waitIdle();
    auto key = *imageBackend->GetImage(0);
    for (VkDescriptorSet set : registeredImGuiTextures[key])
    {
        ImGui_ImplVulkan_RemoveTexture(set);
    }
}

void VulkanContext::RenderToScreen(std::function<void()> renderFunc)
{
    auto &frame = framesInFlight[curFrame];
    vk::ClearValue clearValue;
    clearValue.setColor(vk::ClearColorValue(std::array<float, 4>{0.1, 0.1, 0.1, 1}));
    vk::RenderPassBeginInfo renderPassBegin;
    renderPassBegin.setRenderPass(mainRenderPass->GetBackend().As<VulkanRenderPassBackend>()->GetHandle())
                   .setFramebuffer(*swapchain->GetFrameBuffer(curImageIndex))
                   .setClearValues(clearValue)
                   .setRenderArea(vk::Rect2D({}, {swapchain->GetWidth(), swapchain->GetHeight()}));
    frame->cmdBuf.beginRenderPass(renderPassBegin, vk::SubpassContents::eInline);

    renderFunc();

    frame->cmdBuf.endRenderPass();
}

void VulkanContext::InvalidateSwapchain()
{
    device->GetHandle().waitIdle(); // 确保 GPU 完全空闲

    if (isSurfaceSizeFixed) // Some cases on Linux
    {
        device->GetHandle().waitIdle();
        swapchain = nullptr;
        surface = nullptr;

        VkSurfaceKHR cStyleSurface;
        SDL_Vulkan_CreateSurface(window, *instance, nullptr, &cStyleSurface);
        surface = vk::raii::SurfaceKHR { instance, cStyleSurface };
        auto capabilities = physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface);

        VulkanSwapchainSpecification swapchainSpecification
        {
            .device = Borrow(device),
            .physicalDevice = Borrow(physicalDevice),
            .renderPass = Borrow(mainRenderPass),
            .surface = surface,
            .width = capabilities.currentExtent.width,
            .height = capabilities.currentExtent.height
        };
        swapchain = CreateScope<VulkanSwapchain>(swapchainSpecification);
    }
    else
    {
        swapchain->Invalidate();
    }
}

void VulkanContext::ResizeSwapchain(uint32_t width, uint32_t height)
{
    if (isSurfaceSizeFixed) // Linux
    {
        device->GetHandle().waitIdle();
        swapchain = nullptr;
        surface = nullptr;

        VkSurfaceKHR cStyleSurface;
        SDL_Vulkan_CreateSurface(window, *instance, nullptr, &cStyleSurface);
        surface = vk::raii::SurfaceKHR { instance, cStyleSurface };
        auto capabilities = physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface);

        VulkanSwapchainSpecification swapchainSpecification
        {
            .device = Borrow(device),
            .physicalDevice = Borrow(physicalDevice),
            .renderPass = Borrow(mainRenderPass),
            .surface = surface,
            .width = capabilities.currentExtent.width,
            .height = capabilities.currentExtent.height
        };
        swapchain = CreateScope<VulkanSwapchain>(swapchainSpecification);
    }
    else
    {
        swapchain->Resize(width, height);
    }
}

void VulkanContext::BeginFrame()
{
    auto& frame = framesInFlight[curFrame];
    try {
        if (device->GetHandle().waitForFences(*frame->fence, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
        {
            Log::Error("Wait for Fence Failed");
        }
    } catch (vk::DeviceLostError &e)
    {
        Log::Error("Device Lost Error: {}", e.what());
        return;
    }

    uint32_t imageIndex = 0;
    try
    {
        auto resultValue = swapchain->GetHandle().acquireNextImage(std::numeric_limits<uint64_t>::max(), *frame->imageAvailableSem, nullptr);
        if (resultValue.result == vk::Result::eSuboptimalKHR)
            InvalidateSwapchain();
        imageIndex = resultValue.value;
    }
    catch (const vk::OutOfDateKHRError&)
    {
        InvalidateSwapchain();
        return;
    }
    curImageIndex = imageIndex;

    device->GetHandle().resetFences(*frame->fence);

    frame->cmdBuf.reset();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    frame->cmdBuf.begin(beginInfo);
    
    commandBuffer = frame->cmdBuf;
}

void VulkanContext::EndFrame()
{
    auto &frame = framesInFlight[curFrame];
    frame->cmdBuf.end();

    vk::SubmitInfo submit;
    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit.setCommandBuffers(*frame->cmdBuf)
          .setWaitSemaphores(*frame->imageAvailableSem)
          .setWaitDstStageMask(flags)
          .setSignalSemaphores(*frame->renderFinishedSem);
    try
    {
        device->GetGraphicsQueue().submit(submit, nullptr);
    }
    catch (const vk::SystemError& e)
    {
        Log::Error("Submit failed: {}", e.what());
        InvalidateSwapchain();
        return;
    }
    vk::StructureChain<vk::PresentInfoKHR, vk::SwapchainPresentFenceInfoEXT> presentChain;
    presentChain.get().setWaitSemaphores(*frame->renderFinishedSem)
                      .setSwapchains(*swapchain->GetHandle())
                      .setImageIndices(curImageIndex);
    presentChain.get<vk::SwapchainPresentFenceInfoEXT>().setSwapchainCount(1)
                                                        .setFences(*frame->fence);
    try
    {
        auto result = device->GetPresentQueue().presentKHR(presentChain.get());
        if (result == vk::Result::eSuboptimalKHR)
            InvalidateSwapchain();
    }
    catch (vk::OutOfDateKHRError&)
    {
        InvalidateSwapchain();
    }
    curFrame = (curFrame + 1) % flightCount;
}

void VulkanContext::BindGlobalMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView)
{
    globalMaterialChanged = true;
    currentGlobalMaterial = material;
}
void VulkanContext::BindPassMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView)
{
    passMaterialChanged = true;
    currentPassMaterial = material;
}
void VulkanContext::BindSurfaceMaterial(Borrow<ShaderMaterial> material, Borrow<RenderRegistryView> registryView)
{
    auto backend = material->GetBackend().As<VulkanShaderMaterialBackend>();
    UpdateShaderMaterialDescriptorSet(backend, material->GetResources(), registryView);
    UploadShaderMaterialUniform(backend, material->GetResources());

    auto pipeline = currentPipeline.value()->GetBackend().As<VulkanPipelineBackend>();
    auto bindPoint = PipelineTypeToVulkanBindPoint(currentPipeline.value()->GetPipelineType());
    commandBuffer.bindDescriptorSets(bindPoint, *pipeline->GetPipelineLayout(),
        MaterialSet, *backend->GetDescriptorSet(curFrame), {}, {});
}

void VulkanContext::BindObjectShaderMaterial(uint32_t index, Borrow<ObjectShaderMaterial> material, Borrow<RenderRegistryView> registryView) 
{
    auto backend = material->GetBackend().As<VulkanObjectShaderMaterialBackend>();
    auto schema = registryView->GetShaderMaterialSchema(material->GetSchema());
    // 在填入dynamic UBO时就upload
    std::vector<std::pair<uint32_t, uint32_t>> offsetWithBindings;
    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schema->GetResourceTypeInfos()))
    {
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBufferType = resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>();
            uint32_t perObjectsize = uniformBufferType->GetSize();
            uint32_t stride = AlignUp(perObjectsize, backend->GetMinUniformBufferOffsetAlignment());
            offsetWithBindings.emplace_back(binding, index * stride);
        }
    }
    std::sort(offsetWithBindings.begin(), offsetWithBindings.end(), [](const auto &elem1, const auto &elem2) {
        return elem1.first < elem2.first;
    });
    std::vector<uint32_t> offsets;
    for (auto [_, offset] : offsetWithBindings)
    {
        offsets.push_back(offset);
    }

    auto pipeline = currentPipeline.value()->GetBackend().As<VulkanPipelineBackend>();
    auto bindPoint = PipelineTypeToVulkanBindPoint(currentPipeline.value()->GetPipelineType());
    commandBuffer.bindDescriptorSets(bindPoint, *pipeline->GetPipelineLayout(),
        ObjectShaderMaterialSet, *backend->GetDescriptorSet(curFrame), offsets, {});
}

void VulkanContext::BindGlobalMaterialInPipeline(Borrow<RenderRegistryView> registryView)
{
    auto backend = currentGlobalMaterial.value()->GetBackend().As<VulkanShaderMaterialBackend>();
    UpdateShaderMaterialDescriptorSet(backend, currentGlobalMaterial.value()->GetResources(), registryView);
    UploadShaderMaterialUniform(backend, currentGlobalMaterial.value()->GetResources());

    auto pipeline = currentPipeline.value()->GetBackend().As<VulkanPipelineBackend>();
    auto bindPoint = PipelineTypeToVulkanBindPoint(currentPipeline.value()->GetPipelineType());
    commandBuffer.bindDescriptorSets(bindPoint, *pipeline->GetPipelineLayout(),
        GlobalShaderMaterialSet, *backend->GetDescriptorSet(curFrame), {}, {});
}
void VulkanContext::BindPassMaterialInPipeline(Borrow<RenderRegistryView> registryView)
{
    auto backend = currentPassMaterial.value()->GetBackend().As<VulkanShaderMaterialBackend>();
    UpdateShaderMaterialDescriptorSet(backend, currentPassMaterial.value()->GetResources(), registryView);
    UploadShaderMaterialUniform(backend, currentPassMaterial.value()->GetResources());

    auto pipeline = currentPipeline.value()->GetBackend().As<VulkanPipelineBackend>();
    auto bindPoint = PipelineTypeToVulkanBindPoint(currentPipeline.value()->GetPipelineType());
    commandBuffer.bindDescriptorSets(bindPoint, *pipeline->GetPipelineLayout(),
        PassMaterialSet, *backend->GetDescriptorSet(curFrame), {}, {});
}

void VulkanContext::DrawIndexed(Borrow<VertexBuffer> vertexBuffer, Borrow<IndexBuffer> indexBuffer) 
{
    commandBuffer.bindVertexBuffers(0, *vertexBuffer->GetBackend().As<VulkanVertexBufferBackend>()->GetHandle(), 0ul);
    commandBuffer.bindIndexBuffer(indexBuffer->GetBackend().As<VulkanIndexBufferBackend>()->GetHandle(), 0ul, vk::IndexType::eUint32);
    commandBuffer.drawIndexed(indexBuffer->GetCount(), 1, 0, 0, 0);
}

void VulkanContext::BindGraphicsPipeline(Borrow<GraphicsPipeline> pipeline, Borrow<RenderRegistryView> registryView) 
{
    auto pipelineBackend = pipeline->GetBackend().As<VulkanPipelineBackend>();
    currentPipeline = pipeline;
    auto bindPoint = PipelineTypeToVulkanBindPoint(pipeline->GetPipelineType());
    commandBuffer.bindPipeline(bindPoint, *pipelineBackend->GetPipeline());

    if (globalMaterialChanged)
    {
        BindGlobalMaterialInPipeline(registryView);
        globalMaterialChanged = false;
    }
    if (passMaterialChanged)
    {
        BindPassMaterialInPipeline(registryView);
        passMaterialChanged = false;
    }

    // 动态视口设置
    vk::Viewport viewport;
    viewport.setX(0.0f).setY(0.0f)
            .setWidth(currentFrameBuffer.value()->GetWidth())
            .setHeight(currentFrameBuffer.value()->GetHeight())
            .setMinDepth(0.0f)
            .setMaxDepth(1.0f);
    vk::Rect2D scissor;
    scissor.setOffset({0, 0})
           .setExtent({currentFrameBuffer.value()->GetWidth(), currentFrameBuffer.value()->GetHeight()});
    commandBuffer.setViewport(0, viewport, {});
    commandBuffer.setScissor(0, scissor, {});
}

void VulkanContext::BeginRenderPass(Borrow<RenderPass> renderPass, Borrow<FrameBuffer> frameBuffer, const std::vector<ClearValue> &clearValues) 
{
    auto renderPassBackend = renderPass->GetBackend().As<VulkanRenderPassBackend>();
    auto frameBufferBackend = frameBuffer->GetBackend().As<VulkanFrameBufferBackend>();

    vk::RenderPassBeginInfo vkBeginInfo;
    std::vector<vk::ClearValue> vkClearValues;
    for (auto &clearValue : clearValues)
    {
        vkClearValues.push_back(ClearValueToVulkanType(clearValue));
    }
    vkBeginInfo.setRenderPass(renderPassBackend->GetHandle())
               .setRenderArea(vk::Rect2D{{0,0}, {frameBuffer->GetWidth(), frameBuffer->GetHeight()}})
               .setFramebuffer(*frameBufferBackend->GetFrameBuffer(curFrame))
               .setClearValues(vkClearValues);
    commandBuffer.beginRenderPass(vkBeginInfo, vk::SubpassContents::eInline);

    currentFrameBuffer = frameBuffer;
}

void VulkanContext::EndRenderPass() 
{
    commandBuffer.endRenderPass();
    currentFrameBuffer = std::nullopt;
}

void VulkanContext::WaitIdle() 
{
    device->GetHandle().waitIdle();
}

}
