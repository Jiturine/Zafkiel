#include "Function/RHI/Backends/Vulkan/VulkanPipeline.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanShader.h"
#include "Function/RHI/Backends/Vulkan/VulkanTexture.h"
#include "Function/RHI/Backends/Vulkan/VulkanBuffer.h"

namespace Zafkiel 
{

vk::PipelineBindPoint PipelineTypeToVulkanBindPoint(PipelineType type)
{
    switch (type)
    {
        using enum PipelineType;
    case Graphics: return vk::PipelineBindPoint::eGraphics;
    case Compute: return vk::PipelineBindPoint::eCompute;
    default:
        Log::Error("Unsupported Pipeline Type!");
        return vk::PipelineBindPoint::eGraphics;
    }
}

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


VulkanPipeline::VulkanPipeline()
    : pipelineLayout(nullptr), pipeline(nullptr) 
{
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const RHIGraphicsPipelineDesc &desc, VulkanDevice &device)
    : RHIGraphicsPipeline(desc), device(device)
{
    std::vector<RHIShader *> tmpShaders;
    for (auto shader : desc.shaders)
        tmpShaders.push_back(shader);

    pipelineState = CreateScope<VulkanPipelineState>(tmpShaders, *this, device);

    vk::GraphicsPipelineCreateInfo createInfo;

    // 顶点输入阶段
    vk::PipelineVertexInputStateCreateInfo vertexInputState;

    auto vertexShader = static_cast<VulkanVertexShader *>(desc.shaders[ShaderStage::Vertex]);

    auto &vertexInput = vertexShader->GetVertexInput();
    std::vector<vk::VertexInputAttributeDescription> attributes(vertexInput.inputs.size());
    uint32 stride = 0;
    for (size_t i = 0; i < attributes.size(); i++)
    {
        auto &element = vertexInput.inputs[i];
        attributes[i].setBinding(0)
                     .setFormat(ShaderDataTypeToVulkanFormat(element.type))
                     .setLocation(i)
                     .setOffset(element.offset);
        stride += element.size;
    }
    vk::VertexInputBindingDescription binding;
    binding.setBinding(0)
           .setInputRate(vk::VertexInputRate::eVertex)
           .setStride(vertexInput.stride);

    vertexInputState.setVertexAttributeDescriptions(attributes);
    vertexInputState.setVertexBindingDescriptions(binding);

    createInfo.setPVertexInputState(&vertexInputState);

    // 顶点组装阶段
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.setPrimitiveRestartEnable(false)
                 .setTopology(PrimitiveTopologyToVulkanType(desc.primitiveTopology));
    createInfo.setPInputAssemblyState(&inputAssembly);

    // 配置着色器
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(2);
    shaderStages[0].setStage(vk::ShaderStageFlagBits::eVertex)
                   .setModule(static_cast<VulkanVertexShader *>(desc.shaders[ShaderStage::Vertex])->GetShaderModule())
                   .setPName("main");
    shaderStages[1].setStage(vk::ShaderStageFlagBits::eFragment)
                   .setModule(static_cast<VulkanFragmentShader *>(desc.shaders[ShaderStage::Fragment])->GetShaderModule())
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
        .setCullMode(CullModeToVulkanType(desc.cullMode))
        .setFrontFace(FrontFaceToVulkanType(desc.frontFace))
        .setPolygonMode(PolygonModeToVulkanType(desc.polygonMode))
        .setLineWidth(1);
    createInfo.setPRasterizationState(&rast);

    // 配置多重采样
    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.setSampleShadingEnable(false)
               .setRasterizationSamples(vk::SampleCountFlagBits::e1);
    createInfo.setPMultisampleState(&multisample);

    // 融混阶段
    vk::PipelineColorBlendStateCreateInfo blend;
    std::vector<vk::PipelineColorBlendAttachmentState> attachments(desc.renderTargetDesc.colorAttachmentDescs.size());
    for (size_t i = 0; i < desc.renderTargetDesc.colorAttachmentDescs.size(); i++)
    {
        attachments[i].setBlendEnable(false)
                      .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    }
    blend.setLogicOpEnable(false)
         .setAttachments(attachments);
    createInfo.setPColorBlendState(&blend);

    // 深度测试
    vk::PipelineDepthStencilStateCreateInfo depthStencil;
    depthStencil.setDepthTestEnable(desc.depthTest)
                .setDepthWriteEnable(true)
                .setDepthCompareOp(vk::CompareOp::eLess)
                .setDepthBoundsTestEnable(false); // TODO: 设置
                // .setMinDepthBounds(0.0f)
                // .setMaxDepthBounds(1.0f);
    createInfo.setPDepthStencilState(&depthStencil);

    // 设置layout
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    std::vector<vk::DescriptorSetLayout> setLayouts;
    {
        auto vertexShader = static_cast<VulkanVertexShader *>(desc.shaders[ShaderStage::Vertex]);
        auto &setLayout = device.GetDescriptorManager().GetOrCreateDescriptorSetLayout(vertexShader);
        setLayouts.push_back(*setLayout);
    }

    {
        auto fragmentShader = static_cast<VulkanFragmentShader *>(desc.shaders[ShaderStage::Fragment]);
        auto &setLayout = device.GetDescriptorManager().GetOrCreateDescriptorSetLayout(fragmentShader);
        setLayouts.push_back(*setLayout);
    }
    pipelineLayoutCreateInfo.setSetLayouts(setLayouts);
    
    pipelineLayout = device.GetHandle().createPipelineLayout(pipelineLayoutCreateInfo);

    VulkanRenderTargetInfo renderTargetInfo(desc.renderTargetDesc);

    auto renderPass = device.GetRenderPassManager().GetOrCreateRenderPass(renderTargetInfo);

    createInfo.setRenderPass(renderPass->GetHandle())
              .setSubpass(0)
              .setLayout(pipelineLayout);

    pipeline = device.GetHandle().createGraphicsPipeline(nullptr, createInfo);
}

static VulkanShader *RHIShaderToVulkanShader(RHIShader *shader)
{
    switch (shader->GetShaderType())
    {
        using enum ShaderType;
    case Vertex: return static_cast<VulkanVertexShader *>(static_cast<RHIVertexShader *>(shader));
    case Fragment: return static_cast<VulkanFragmentShader *>(static_cast<RHIFragmentShader *>(shader));
    // case Geometry: return static_cast<VulkanGeometryShader *>(static_cast<RHIGeometryShader *>(shader));
    default:
        Log::Error("Unknown ShaderType!");
        return nullptr;
    }
}

VulkanPipelineState::VulkanPipelineState(const std::vector<RHIShader *> &shaders, VulkanPipeline &pipeline, VulkanDevice &device)
    : device(device), pipeline(pipeline), shaders(shaders)
{
    writers.resize(shaders.size());
    currentBufferInfos.resize(shaders.size());
    currentImageInfos.resize(shaders.size());

    // 统计 总的 WriteDescriptorSet 数量
    uint32 totalWrites = 0;
    for (auto shader : shaders)
    {
        totalWrites += shader->GetResourceTable().resourceTypeInfos.size();
    }
    writeInfos.resize(totalWrites);

    std::vector<std::pair<uint32, uint32>> dynamicUniformBufferLocs;

    int32 currentWriteIndex = 0;
    for (auto [set, shader] : std::views::enumerate(shaders))
    {
        auto &SRT = shader->GetResourceTable();
        writeDescriptorContainers.emplace_back(SRT.bufferCount, SRT.imageCount);

        if (currentWriteIndex < totalWrites)  // 若currentWriteIndex为totalWrites + 1，会越界
            writers[set] = &writeInfos[currentWriteIndex];

        // 预先分配空间，大小等于该shader的所有资源数量
        currentBufferInfos[set].resize(SRT.resourceTypeInfos.size());
        currentImageInfos[set].resize(SRT.resourceTypeInfos.size());

        for (auto [binding, resourceTypeInfo] : std::views::enumerate(SRT.resourceTypeInfos))
        {
            vk::WriteDescriptorSet writeInfo;
            writeInfo.setDstBinding(binding)
                     .setDescriptorCount(1);
            // dstSet 在UpdateDescriptorSets时 从cache传入

            switch (resourceTypeInfo.type->GetCategory())
            {
                using enum ShaderReflection::ResourceTypeCategory;
            case UniformBlock:
            {
                vk::DescriptorBufferInfo bufferInfo;
                writeDescriptorContainers.back().bufferInfos.push_back(bufferInfo);

                writeInfo.setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
                         .setBufferInfo(writeDescriptorContainers.back().bufferInfos.back());

                dynamicUniformBufferLocs.emplace_back(set, binding);

                auto uniformBlock = resourceTypeInfo.type->As<ShaderReflection::UniformBlock>();
                uniformBlockSizes[std::make_pair(set, binding)] = uniformBlock->GetSize();

                break;
            }

            case SampledImage:
            {
                vk::DescriptorImageInfo imageInfo;
                writeDescriptorContainers.back().imageInfos.push_back(imageInfo);

                writeInfo.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                         .setImageInfo(writeDescriptorContainers.back().imageInfos.back());
                break;
            }

            default:
                Log::Error("Unknown ResourceType!");
                break;
            }

            writeInfos[currentWriteIndex++] = writeInfo;
        }
    }
    
    std::sort(dynamicUniformBufferLocs.begin(), dynamicUniformBufferLocs.end(), 
        [](std::pair<uint32, uint32> &a, std::pair<uint32, uint32> &b) {
        return a.first < b.first || (a.first == b.first && a.second < b.second);
    });

    dynamicOffsets.resize(dynamicUniformBufferLocs.size());

    for (auto [index, p] : std::views::enumerate(dynamicUniformBufferLocs))
    {
        dynamicOffsetMap[p] = index;
    }
}

void VulkanPipelineState::UpdateDescriptorSets(const std::vector<vk::DescriptorSet> &descriptorSets)
{
    uint32 writeIndex = 0;
    for (size_t set = 0; set < shaders.size(); set++)
    {
        for (size_t binding = 0; binding < shaders[set]->GetResourceTable().resourceTypeInfos.size(); binding++)
        {
            writeInfos[writeIndex].setDstSet(descriptorSets[set]);
            writeIndex++;
        }
    }

    device.GetHandle().updateDescriptorSets(writeInfos, nullptr);

    resourceDirty = false;
}


void VulkanPipelineState::BindDescriptorSets(VulkanCommandBuffer &commandBuffer)
{
    std::vector<vk::DescriptorSet> descriptorSets;
    for (size_t i = 0; i < shaders.size(); i++)
    {
        auto vkShader = RHIShaderToVulkanShader(shaders[i]);
        vk::DescriptorSet descriptorSet = device.GetDescriptorManager().GetOrCreateDescriptorSet(
            vkShader,
            currentBufferInfos[i],
            currentImageInfos[i]
        );
        descriptorSets.push_back(descriptorSet);
    }

    if (resourceDirty)
    {
        UpdateDescriptorSets(descriptorSets);
    }

    commandBuffer.GetHandle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, // TODO: 区分Graphics和Compute
        pipeline.GetPipelineLayout(), 0, descriptorSets, dynamicOffsets);
}

void VulkanPipelineState::SetDynamicOffset(uint32 set, uint32 binding, uint32 offset)
{
    uint32 index = dynamicOffsetMap[std::make_pair(set, binding)];

    dynamicOffsets[index] = offset;
}

void VulkanPipelineState::SetUniformBuffer(uint32 set, uint32 binding, VulkanBuffer *uniformBuffer)
{
    uint32 range = uniformBlockSizes.at(std::make_pair(set, binding));

    auto oldBufferInfo = const_cast<vk::DescriptorBufferInfo*>(writers[set][binding].pBufferInfo);

    if (oldBufferInfo->buffer != *uniformBuffer->GetBuffer())
    {
        oldBufferInfo->setBuffer(uniformBuffer->GetBuffer())
                      .setOffset(0)
                      .setRange(range);

        currentBufferInfos[set][binding] = uniformBuffer;

        resourceDirty = true;
    }
}

// TODO: 对未设置的 image 使用默认sampler，image view等
void VulkanPipelineState::SetTexture(uint32 set, uint32 binding, VulkanTexture *texture)
{
    auto oldImageInfo = const_cast<vk::DescriptorImageInfo*>(writers[set][binding].pImageInfo);

    if (oldImageInfo->imageView != *texture->GetImageView())
    {
        oldImageInfo->setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                     .setImageView(texture->GetImageView())
                     .setSampler(texture->GetSampler());

        currentImageInfos[set][binding] = texture;

        resourceDirty = true;
    }
}

void VulkanPipelineState::ClearResources()
{
for (auto &bufferInfos : currentBufferInfos)                                                                                                                                                                                                                 
      {                                                                                                                                                                                                                                                            
          for (auto &bufferInfo : bufferInfos)                                                                                                                                                                                                                     
          {                                                                                                                                                                                                                                                        
              bufferInfo = std::nullopt;                                                                                                                                                                                                                           
          }                                                                                                                                                                                                                                                        
      }                                                                                                                                                                                                                                                            
                  
      for (auto &imageInfos : currentImageInfos)                                                                                                                                                                                                                   
      {
          for (auto &imageInfo : imageInfos)                                                                                                                                                                                                                       
          {                                                                                                                                                                                                                                                        
              imageInfo = std::nullopt;                                                                                                                                                                                                                            
          }                                                                                                                                                                                                                                                        
      }     
}

void VulkanGraphicsPipeline::SetDynamicOffsetIndex(ShaderStage::Stage stage, const std::string &name, uint32 index) 
{
    auto &SRT = desc.shaders[stage]->GetResourceTable();
    uint32 set = stage;
    uint32 binding = SRT.resourceNameToBinding[name];
    
    auto uniformBlock = SRT.resourceTypeInfos[binding].type->As<ShaderReflection::UniformBlock>();
    uint32 stride = AlignUp(uniformBlock->GetSize(), device.GetMinUniformBufferOffsetAlignment());
    uint32 offset = stride * index;

    pipelineState->SetDynamicOffset(set, binding, offset);
}

void VulkanGraphicsPipeline::SetUniformBuffer(ShaderStage::Stage stage, const std::string &name, RHIBuffer *buffer) 
{
    auto &SRT = desc.shaders[stage]->GetResourceTable();
    uint32 set = stage;
    uint32 binding = SRT.resourceNameToBinding[name];

    pipelineState->SetUniformBuffer(set, binding, static_cast<VulkanBuffer *>(buffer));
}

void VulkanGraphicsPipeline::SetTexture(ShaderStage::Stage stage, const std::string &name, RHITexture *texture) 
{
    auto &SRT = desc.shaders[stage]->GetResourceTable();
    uint32 set = stage;
    uint32 binding = SRT.resourceNameToBinding[name];

    pipelineState->SetTexture(set, binding, static_cast<VulkanTexture *>(texture));
}

}