#include "function/render/shader_material_schema.h"
#include "function/render/shader_compiler/glsl_schema_parser.h"

namespace Zafkiel
{

ShaderMaterialSchema::ShaderMaterialSchema(const Path &path)
{
    std::string source = FileSystem::ReadText(path);
    GlslSchemaParser parser(source);
    resourceTypeInfos = std::move(parser.GetResourceTypeInfos());
    resourceNameToBinding = std::move(parser.GetResourceNameToBinding());
    customDataTypes = std::move(parser.GetCustomDataTypes());
    customResourceTypes = std::move(parser.GetCustomResourceTypes());
    aliases = std::move(parser.GetAliases());
}

const ShaderReflection::DataType *ShaderMaterialSchema::GetUniformParameterType(const std::string &path) const
{
    auto parsedPath = ParseParameterPath(path);
    auto binding = resourceNameToBinding.at(parsedPath.elems[0].name);
    auto uniformBlock = resourceTypeInfos.at(binding).value().type->As<ShaderReflection::UniformBlock>();
    const ShaderReflection::DataType *curType = uniformBlock->GetType();
    for (uint32_t i = 1; i < parsedPath.elems.size(); i++)
    {
        if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
        {
            return curType;
        }
        auto elem = parsedPath.elems[i];
        if (elem.type == ShaderMaterialParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curType->As<ShaderReflection::Struct>()->GetFields();
            bool find = false;
            for (auto &field : fields)
            {
                if (field->GetName() == elem.name)
                {
                    curType = field->GetTypeInfo();
                    find = true;
                    break;
                }
            }
            if (!find)
            {
                Log::Error("ParameterPath Unavailable!");
                return nullptr;
            }
        }
        else if (elem.type == ShaderMaterialParameterPath::PathElemType::Index && curType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
        {
            // TODO: deal with array;
            Log::Error("TODO array");
            return nullptr;
        }
    }
    return curType;
}

}
