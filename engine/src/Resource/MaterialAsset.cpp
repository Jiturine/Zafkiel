#include "Resource/MaterialAsset.h"
#include "Resource/AssetManager.h"
#include "Resource/ShaderAsset.h"
#include "Core/Meta/Serializer/YamlSerializer.h"

namespace Zafkiel
{
MaterialAsset::MaterialAsset(AssetHandle handle, const std::string &name, const std::string &data)
    : Asset(handle), name(name)
{
    YamlDeserializer materialData(data);

    shaderFamily = materialData["ShaderFamily"].As<ShaderFamily>();

    for (auto &[paramName, paramData] : materialData["Parameters"].MapItems())
    {
        std::string name = paramName.As<std::string>();
        auto paramType = paramData["Type"].As<MaterialAssetParameterType>();
        auto &paramValue = paramData["Value"];
        switch (paramType)
        {
            using enum MaterialAssetParameterType;
        case SampledImage: parameters[name] = MaterialAssetParameter(paramType, paramValue.As<AssetHandle>()); break;
        case Vec3: parameters[name] = MaterialAssetParameter(paramType, paramValue.As<vec3>()); break;
        case Vec4: parameters[name] = MaterialAssetParameter(paramType, paramValue.As<vec4>()); break;
        case Mat3: parameters[name] = MaterialAssetParameter(paramType, paramValue.As<mat3>()); break;
        case Mat4: parameters[name] = MaterialAssetParameter(paramType, paramValue.As<mat4>()); break;
        default: 
            Log::Error("Unknown MaterialAssetParameterType!");
            break;
        }
    }
}

std::string MaterialAsset::Serialize()
{
    YamlSerializer serializer;
    serializer.BeginMap();
    serializer.Key("Parameters").BeginMap();

    for (auto [paramName, paramData] : parameters)
    {
        serializer.Key(paramName);
        serializer.BeginMap();
        serializer.Key("Type").Value(paramData.type);
        serializer.Key("Value");
        switch (paramData.type)
        {
            using enum MaterialAssetParameterType;
        case SampledImage: serializer.Value(paramData.assetHandle); break;
        case Vec3: serializer.Value(paramData.vec3Value); break;
        case Vec4: serializer.Value(paramData.vec4Value); break;
        case Mat3: serializer.Value(paramData.mat3Value); break;
        case Mat4: serializer.Value(paramData.mat4Value); break;
        default: 
            Log::Error("Unknown MaterialAssetParameterType!");
            break;
        }
        serializer.EndMap();
    }
    serializer.EndMap().EndMap();

    return serializer.c_str();
}

void MaterialAsset::SetTexture2D(const std::string &paramName, AssetHandle texture)
{
    parameters[paramName] = MaterialAssetParameter(MaterialAssetParameterType::SampledImage, texture);
}

}
