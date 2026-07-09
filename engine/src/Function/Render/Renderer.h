#pragma once
#include "Function/RHI/RHI.h"
#include <imgui.h>
#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Application/Application.h"

#include "Function/RHI/ShaderCompiler/GlslCompiler.h"

#include "Function/Render/RenderTargetPool.h"
#include "Function/Render/Mesh.h"
#include "Function/Render/Material.h"

#include "Resource/Asset.h"

namespace Zafkiel
{
class PlatformWindow;
class MaterialAsset;
class MeshAsset;
class Texture2DAsset;
class FontAsset;

class Renderer : public Singleton<Renderer, true>
{
  public:
    Renderer(GraphicsAPI API);

    ~Renderer();

    std::vector<ImTextureRef> RegisterImGuiTexture(RHITexture *texture);

    void UnregisterImGuiTexture(RHITexture *texture);

    void DestroyImGui();

    RenderTargetPool &GetRenderTargetPool() { return *renderTargetPool.get(); }

    Ref<Mesh> GetOrCreateMesh(AssetHandle meshAssetHandle);

    Ref<Material> GetOrCreateMaterial(AssetHandle materialAssetHandle, const MaterialDesc &desc);

  private:
    Scope<RenderTargetPool> renderTargetPool;

    std::unordered_map<AssetHandle, Ref<Mesh>> meshes;

    std::unordered_map<AssetHandle, Ref<Material>> materials;

};

}
