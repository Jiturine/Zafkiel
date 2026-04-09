#pragma once
#include "Core/Base/Memory.h"
#include "Resource/Asset.h"
#include "Resource/AssetManager.h"
#include "Resource/Texture2DAsset.h"
#include "Function/Render/Renderer.h"

namespace Zafkiel
{

enum class [[refl]] ShaderFamily
{
    Unlit,
    BlinnPhong,
    PBR
};

enum class [[refl]] MaterialAssetParameterType
{
    None,
    SampledImage,
    Vec3, Vec4,
    Mat3, Mat4,
};

struct MaterialAssetParameter
{
    MaterialAssetParameter() : type(MaterialAssetParameterType::None), assetHandle() {}

    MaterialAssetParameter(MaterialAssetParameterType type, AssetHandle assetHandle)
        : type(type), assetHandle(assetHandle) {}

    MaterialAssetParameter(MaterialAssetParameterType type, vec3 vec3Value)
        : type(type), vec3Value(vec3Value) {}

    MaterialAssetParameter(MaterialAssetParameterType type, vec4 vec4Value)
        : type(type), vec4Value(vec4Value) {}

    MaterialAssetParameter(MaterialAssetParameterType type, mat3 mat3Value)
        : type(type), mat3Value(mat3Value) {}

    MaterialAssetParameter(MaterialAssetParameterType type, mat4 mat4Value)
        : type(type), mat4Value(mat4Value) {}
        
    MaterialAssetParameterType type;
    union
    {
        AssetHandle assetHandle;
        vec3 vec3Value;
        vec4 vec4Value;
        mat3 mat3Value;
        mat4 mat4Value;
    };
};

class MaterialAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Material; }

    // 从序列化文件构造
    MaterialAsset(AssetHandle handle, const std::string &name, const std::string &data);

    // 从零构造
    MaterialAsset(AssetHandle handle, const std::string &name, ShaderFamily shaderFamily)
        : Asset(handle), name(name), shaderFamily(shaderFamily) {}

    std::string GetName() const { return name; }
    
    std::string Serialize();

    std::unordered_map<std::string, MaterialAssetParameter> &GetParameters() { return parameters; }

    void SetTexture2D(const std::string &paramName, AssetHandle texture);

  private:
    std::string name;
    ShaderFamily shaderFamily;
    std::unordered_map<std::string, MaterialAssetParameter> parameters;
};

}

