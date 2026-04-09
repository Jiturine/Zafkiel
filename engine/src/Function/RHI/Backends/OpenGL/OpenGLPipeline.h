#pragma once
#include "Function/RHI/RHIResources.h"
#include <glad/glad.h>

namespace Zafkiel
{

class OpenGLBuffer;
class OpenGLTexture;

GLenum PrimitiveTopologyToOpenGLType(PrimitiveTopology type);

GLenum CullModeToOpenGLType(CullMode mode);

GLenum FrontFaceToOpenGLType(FrontFace type);

GLenum PolygonModeToOpenGLType(PolygonMode mode);

struct OpenGLPendingPipelineResources
{
    std::unordered_map<uint32, OpenGLBuffer *> uniformBuffers;

    std::unordered_map<uint32, std::optional<uint32>> dynamicUniformBufferIndices;

    std::unordered_map<uint32, OpenGLTexture *> textures;
};

class OpenGLGraphicsPipeline : public RHIGraphicsPipeline
{
  public:
    OpenGLGraphicsPipeline(const RHIGraphicsPipelineDesc &desc);

    void SetPendingResources();

    virtual void SetDynamicOffsetIndex(ShaderStage::Stage stage, const std::string &name, uint32 index) override;

    virtual void SetUniformBuffer(ShaderStage::Stage stage, const std::string &name, RHIBuffer *buffer) override;

    virtual void SetTexture(ShaderStage::Stage stage, const std::string &name, RHITexture *texture) override;

    virtual void ClearResources() override;

    GLuint GetShaderProgram() const { return shaderProgram; }

  private:
    GLuint shaderProgram;
  
    std::unordered_map<uint32, OpenGLPendingPipelineResources> pendingResources;
};

}
