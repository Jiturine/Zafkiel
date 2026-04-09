#include "Function/Render/Material.h"
#include "Function/RHI/RHICommandList.h"
#include "Resource/AssetManager.h"
#include "Resource/MaterialAsset.h"

namespace Zafkiel
{

Material::Material(AssetHandle materialAssetHandle, const MaterialDesc &desc)
{
    auto materialAsset = AssetManager::Instance().GetAsset(materialAssetHandle).As<MaterialAsset>();

    auto &materialParams = materialAsset->GetParameters();

    for (auto &uniformBlockInfo : desc.uniformBlocks)
    {
        RHIBufferDesc uniformBufferDesc
        {
            .size = uniformBlockInfo.type->GetSize(),
            .usages = BufferUsageFlags::UniformBuffer | BufferUsageFlags::Dynamic | BufferUsageFlags::CPUAccessible,
        };

        auto uniformBuffer = GlobalRHICmdList->CreateBuffer(uniformBufferDesc);

        uniformBuffers[uniformBlockInfo.name] = uniformBuffer;

        auto uniformBufferContent = CreateRef<UniformBufferContent>(uniformBlockInfo.type);
        
        uniformBufferContents[uniformBlockInfo.name] = uniformBufferContent;

        for (auto &[materialParamName, uniformBufferParamName] : uniformBlockInfo.paramMap)
        {
            auto materialParamValue = materialParams[materialParamName];

            switch (materialParamValue.type)
            {
                using enum MaterialAssetParameterType;
            case Vec3: uniformBufferContent->SetParameter(uniformBufferParamName, materialParamValue.vec3Value, ShaderFundamentalType::Float3); break;
            case Vec4: uniformBufferContent->SetParameter(uniformBufferParamName, materialParamValue.vec4Value, ShaderFundamentalType::Float4); break;
            case Mat3: uniformBufferContent->SetParameter(uniformBufferParamName, materialParamValue.mat3Value, ShaderFundamentalType::Mat3); break;
            case Mat4: uniformBufferContent->SetParameter(uniformBufferParamName, materialParamValue.mat4Value, ShaderFundamentalType::Mat4); break;
            default:
                Log::Error("Invalid MaterialAssetParameterType!");
                break;
            }
        }
    }

    for (auto &[materialTextureName, shaderTextureName] : desc.textureMap)
    {
        if (!materialParams.contains(materialTextureName))
        {
            Log::Warn("Material doesn't have texture: {} !", materialTextureName);
            continue;
        }

        auto textureAsset = AssetManager::Instance().GetAsset(materialParams[materialTextureName].assetHandle).As<Texture2DAsset>();

        RHITextureDesc textureDesc
        {
            .width = textureAsset->GetWidth(),
            .height = textureAsset->GetHeight(),
            .format = textureAsset->GetFormat(),
            .usages = ImageUsageFlags::Sampled | ImageUsageFlags::Upload,
            // TODO: 其他参数也应该在TextureAsset中
        };

        textures[shaderTextureName] = GlobalRHICmdList->CreateTexture(textureDesc, textureAsset->GetBuffer());
    }
}

}