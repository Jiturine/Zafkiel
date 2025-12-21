#pragma once

#include "function/render/shader.h"
#include "resource/asset.h"

namespace Zafkiel
{
class ShaderAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Shader; }

    ShaderAsset(AssetHandle handle, const Path &path)
        : Asset(handle)
    {
        // Renderer::Submit([self = Ref(this), path]() mutable {
        //     self->shader = Renderer::GetGraphicsContext()->CreateGraphicsShader(path);
        // });
    }

    ~ShaderAsset()
    {
        // Renderer::Submit([shader = std::move(shader)]() mutable {
        //     shader = nullptr;
        // });
    }

    // Observer<Shader> GetShader() { return shader; }

  private:
    // Scope<Shader> shader;
    
};
}
