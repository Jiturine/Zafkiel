#include "vulkan_graphics_pipeline.h"
#include "vulkan_object_render_resource_template.h"
#include "vulkan_render_resource_template.h"
#include "vulkan_graphics_shader.h"
#include "vulkan_vertex_buffer.h"
#include "vulkan_render_pass.h"

namespace Zafkiel
{

vk::PrimitiveTopology PrimitiveTopologyToVulkanType(PrimitiveTopology type)
{
    switch (type)
    {
        using enum PrimitiveTopology;
	case Points: return vk::PrimitiveTopology::ePointList;
	case Lines: return vk::PrimitiveTopology::eLineList;
	case Triangles: return vk::PrimitiveTopology::eTriangleList;
	case LineStrip: return vk::PrimitiveTopology::eLineStrip;
	case TriangleStrip: return vk::PrimitiveTopology::eTriangleStrip;
	case TriangleFan: return vk::PrimitiveTopology::eTriangleFan;
    default:
        Log::Error("Unknown Primitive Topology!");
        return vk::PrimitiveTopology::ePointList;
    }
}

vk::CullModeFlagBits CullModeToVulkanType(CullMode mode)
{
    switch (mode)
    {
        using enum CullMode;
    case Front: return vk::CullModeFlagBits::eFront;
    case Back: return vk::CullModeFlagBits::eBack;
    case FrontAndBack: return vk::CullModeFlagBits::eFrontAndBack;
    case None: return vk::CullModeFlagBits::eNone;
    default: return vk::CullModeFlagBits::eNone;
    }
}

vk::FrontFace FrontFaceToVulkanType(FrontFace type)
{
    switch (type)
    {
        using enum FrontFace;
    case CounterClockWise: return vk::FrontFace::eCounterClockwise;
    case ClockWise: return vk::FrontFace::eClockwise;
    default: return vk::FrontFace::eCounterClockwise;
    }
}

vk::PolygonMode PolygonModeToVulkanType(PolygonMode mode)
{
    switch (mode)
    {
        using enum PolygonMode;
    case Fill: return vk::PolygonMode::eFill;
    case Wireframe: return vk::PolygonMode::eLine;
    default: return vk::PolygonMode::eFill;
    }
}

VulkanGraphicsPipelineBackend::VulkanGraphicsPipelineBackend(const GraphicsPipelineSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPipelineBackend> &pipelineBackend)
{
    vk::GraphicsPipelineCreateInfo createInfo;

    // 顶点输入阶段
    vk::PipelineVertexInputStateCreateInfo vertexInputState;

    auto &vertexBufferLayout = spec.shader->GetReflection().vertexInput;
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
                   .setModule(spec.shader->GetVertexModule()->GetShaderModuleBackend().As<VulkanShaderModuleBackend>()->GetHandle())
                   .setPName("main");
    shaderStages[1].setStage(vk::ShaderStageFlagBits::eFragment)
                   .setModule(spec.shader->GetFragmentModule()->GetShaderModuleBackend().As<VulkanShaderModuleBackend>()->GetHandle())
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
    for (auto &renderResourceTemplate : spec.renderResourceTemplates)
    {
        if (auto *commonTemplate = std::get_if<Observer<RenderResourceTemplate>>(&renderResourceTemplate); commonTemplate && *commonTemplate)
        {
            setLayouts.push_back(*(*commonTemplate)->GetBackend().As<VulkanRenderResourceTemplateBackend>()->GetDescriptorSetLayout());
        }
        else if (auto *objectTemplate = std::get_if<Observer<ObjectRenderResourceTemplate>>(&renderResourceTemplate); objectTemplate && *objectTemplate)
        {
            setLayouts.push_back(*(*objectTemplate)->GetBackend().As<VulkanObjectRenderResourceTemplateBackend>()->GetDescriptorSetLayout());
        }
        else
        {
            setLayouts.push_back({});
        }
    }
    pipelineLayoutCreateInfo.setSetLayouts(setLayouts);
    pipelineBackend->pipelineLayout = device->GetHandle().createPipelineLayout(pipelineLayoutCreateInfo);

    createInfo.setRenderPass(spec.renderPass->GetBackend().As<VulkanRenderPassBackend>()->GetHandle())
              .setSubpass(0)
              .setLayout(pipelineBackend->pipelineLayout);

    pipelineBackend->pipeline = device->GetHandle().createGraphicsPipeline(nullptr, createInfo);
}

Scope<GraphicsPipeline> VulkanGraphicsPipelineFactory::Create(const GraphicsPipelineSpecification &spec, const Scope<VulkanDevice> &device)
{
    auto pipelineBackend = CreateScope<VulkanPipelineBackend>();
    auto graphicsPipelineBackend = CreateScope<VulkanGraphicsPipelineBackend>(spec, device, pipelineBackend);
    return CreateScope<GraphicsPipeline>(spec, std::move(pipelineBackend), std::move(graphicsPipelineBackend));
}

}
