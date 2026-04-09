#pragma once

#include "Resource/Asset.h"
#include "Function/RHI/RHIResources.h"

namespace Zafkiel 
{

struct MaterialDesc 
{
    struct UniformBlockInfo
    {
        std::string name;
        const ShaderReflection::UniformBlock *type;
        std::unordered_map<std::string, std::string> paramMap;
    };
    std::vector<UniformBlockInfo> uniformBlocks;
    std::unordered_map<std::string, std::string> textureMap;
};

class Material : public RefCounted
{
  public:
    Material(AssetHandle materialAssetHandle, const MaterialDesc &desc);

    RHITexture *GetTexture(const std::string &name) { return textures[name].get(); }

  private:
    std::unordered_map<std::string, Ref<RHIBuffer>> uniformBuffers;
    std::unordered_map<std::string, Ref<UniformBufferContent>> uniformBufferContents;
    std::unordered_map<std::string, Ref<RHITexture>> textures;
};

}