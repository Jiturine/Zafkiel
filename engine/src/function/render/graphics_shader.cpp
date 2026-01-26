#include "function/render/graphics_shader.h"

namespace Zafkiel 
{

void GraphicsShader::CombineModules(Borrow<VertexModule> vertexModule_)
{
    auto &inputs = vertexModule_->GetReflection().inputs;
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
}

}