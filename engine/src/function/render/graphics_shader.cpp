#include "graphics_shader.h"

namespace Zafkiel 
{

void GraphicsShader::CombineModules()
{
    auto &inputs = vertexModule->GetReflection().inputs;
    uint32_t stride = 0;
    for (auto &input : inputs)
    {
        VertexBufferElementLayout elementLayout
        {
            .name = input.name,
            .location = input.location,
            .size = input.size,
            .offset = input.offset,
            .type = input.type
        };
        reflection.vertexInput.elements.push_back(elementLayout);
        stride += input.size;
    }
    reflection.vertexInput.stride = stride;
    
    // std::vector<Observer<ShaderModule>> modules { vertexModule, fragmentModule };

    // for (auto module : modules)
    // {
    //     for (auto &uniformBuffer : module->GetReflection().uniformBuffers)
    //     {
    //         auto it = std::find_if(reflection.uniformBuffers.begin(), reflection.uniformBuffers.end(), [&uniformBuffer](const UniformBufferLayout &layout){ return layout.binding == uniformBuffer.binding; });
    //         if (it == reflection.uniformBuffers.end())
    //         {
    //             reflection.uniformBuffers.push_back(uniformBuffer);
    //         }
    //     }
    //     for (auto &sampledImage : module->GetReflection().sampledImages)
    //     {
    //         auto it = std::find_if(reflection.sampledImages.begin(), reflection.sampledImages.end(), [&sampledImage](const SampledImageLayout &layout){ return layout.binding == sampledImage.binding; });
    //         if (it == reflection.sampledImages.end())
    //         {
    //             reflection.sampledImages.push_back(sampledImage);
    //         }
    //     }
    // }
}

}