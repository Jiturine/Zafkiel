#include "object_render_resource_template.h"
#include "glslang/MachineIndependent/localintermediate.h"
#include "glslang/Public/ResourceLimits.h"
#include "glslang/Public/ShaderLang.h"
#include <fstream>
#include <regex>

namespace Zafkiel 
{

ObjectRenderResourceTemplate::ObjectRenderResourceTemplate(const Path &path, Scope<ObjectRenderResourceTemplateBackend> backend)
    : backend(std::move(backend))
{
    std::ifstream file(path);
    if (!file.is_open()) {
        Log::Error("Could not open the file!");
        return;
    }

    std::string line;
    std::vector<std::string> parametersPart;
    std::string glslPart;

    // 读取文件内容，分为 #parameters 部分 和 GLSL 部分
    bool isParameters = false;
    while (std::getline(file, line)) {
        if (line.find("#parameters") != std::string::npos) {
            isParameters = true; // 开始读取 #parameters 部分
            continue;  // 跳过 #parameters 行
        }

        if (isParameters) {
            if (line.empty() || line.find("#version") != std::string::npos) {
                // 找到 #version 或其他内容，停止读取 #parameters 部分
                isParameters = false;
                glslPart += line + "\n";
            } else {
                parametersPart.push_back(line);
            }
        } else {
            glslPart += line + "\n";  // 读取 GLSL 部分
        }
    }
    ParseAliases(parametersPart);
    ParseShaderSchema(glslPart);
}

void ObjectRenderResourceTemplate::ParseAliases(const std::vector<std::string> &lines)
{
    std::unordered_map<std::string, std::string> table;
    bool inSection = false;

    std::regex r(R"(^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*([^;]+)\s*;)");

    for (auto& line : lines)
    {
        std::smatch m;
        if (std::regex_match(line, m, r)) {
            std::string alias = m[1];
            std::string path  = m[2];
            aliases[alias] = ParseParameterPath(path);
        }
    }
}

const ShaderReflection::DataType *ObjectRenderResourceTemplate::ParseDataType(const glslang::TType *type, const glslang::TIntermediate *intermediate)
{
    if (type->isArray())
    {
       return ParseArrayType(type, intermediate);
    }
    else if (type->isStruct())
    {
        return ParseStructType(type, intermediate);
    }
    else if (type->isScalar() || type->isVector() || type->isMatrix())
    {
        return ParseFundamentalType(type, intermediate);
    }
    else 
    {
        Log::Error("Failed to Parse Data Type!");
        return nullptr;
    }
}

ShaderReflection::UniformFieldLayout ObjectRenderResourceTemplate::CalculateUniformLayout(const std::string &name, const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType)
{
    if (type->isArray())
    {
       return CalculateArrayLayout(name, type, intermediate, customType);
    }
    else if (type->isStruct())
    {
        return CalculateStructLayout(name, type, intermediate, customType);
    }
    else if (type->isScalar() || type->isVector() || type->isMatrix())
    {
        return CalculateFundamentalLayout(name, type, intermediate, customType);
    }
    else 
    {
        Log::Error("Failed to Calculate Uniform Layout!");
        return {};
    }
}

ShaderReflection::UniformFieldLayout ObjectRenderResourceTemplate::CalculateArrayLayout(const std::string &name, const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType)
{
    return {};
}
ShaderReflection::UniformFieldLayout ObjectRenderResourceTemplate::CalculateStructLayout(const std::string &name, const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType)
{
    ShaderReflection::UniformFieldLayout layout
    {
        .name = name,
        .dataType = customType,
        .offset = (uint32_t)type->getQualifier().layoutOffset,
        .size = (uint32_t)intermediate->getBlockSize(*type),
    };

    const glslang::TTypeList* structMembers = type->getStruct();
    auto customStructType = customType->As<ShaderReflection::Struct>();
    uint32_t structSize = 0;
    for (uint32_t i = 0; i < structMembers->size(); i++)
    {
        const auto &memberLoc = (*structMembers)[i];
        auto memberType = memberLoc.type;

        std::string fieldName = memberType->getFieldName().c_str();

        auto memberLayout = CalculateUniformLayout(fieldName, memberType, intermediate, customStructType->GetFields()[i]->GetTypeInfo());
        layout.children.push_back(memberLayout);
    }

    return layout;
}
ShaderReflection::UniformFieldLayout ObjectRenderResourceTemplate::CalculateFundamentalLayout(const std::string &name, const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType)
{
    ShaderReflection::UniformFieldLayout layout
    {
        .name = name,
        .dataType = customType,
        .offset = (uint32_t)type->getQualifier().layoutOffset,
    };
    int size, stride;
    intermediate->getBaseAlignment(*type, size, stride, type->getQualifier().layoutPacking, type->getQualifier().layoutMatrix == glslang::ElmRowMajor);
    layout.size = size;

    if (type->isMatrix())
    {
        layout.matrixStride = stride;
    }

    return layout;
}

RenderResourceDescription ObjectRenderResourceTemplate::ParseResourceType(const glslang::TIntermSymbol *symbol, const glslang::TIntermediate *intermediate)
{
    const glslang::TType& t = symbol->getType();
    const glslang::TString& name = symbol->getName();

    // Skip built-in variables and anonymous blocks
    if (name.find("gl_") == 0 || name.find("anon@") == 0) {
        return {};
    }

    const glslang::TQualifier& q = t.getQualifier();
    uint32_t set = 0, binding = 0;
    
    if (q.hasSet()) set = q.layoutSet;
    if (q.hasBinding()) binding = q.layoutBinding;

    if (q.storage == glslang::EvqUniform && t.getBasicType() == glslang::EbtBlock)
    {
        auto structType = ParseStructType(&t, intermediate)->As<ShaderReflection::Struct>();
        ShaderReflection::LayoutRule layoutRule = q.layoutPacking == glslang::TLayoutPacking::ElpStd140 ? ShaderReflection::LayoutRule::Std140 :
                                                  q.layoutPacking == glslang::TLayoutPacking::ElpStd430 ? ShaderReflection::LayoutRule::Std430 :
                                                  ShaderReflection::LayoutRule::Unknown;

        auto layout = CalculateUniformLayout(name.c_str(), &t, intermediate, structType);
        
        customResourceTypes[name.c_str()] = CreateScope<ShaderReflection::UniformBlock>(name.c_str(), layoutRule, structType, layout);

        return 
        {
            customResourceTypes[name.c_str()].get(),
            set,
            binding
        };
    }
    else 
    {
        Log::Error("Unknown Resource Type!");
        return {};
    }
}

const ShaderReflection::DataType *ObjectRenderResourceTemplate::ParseArrayType(const glslang::TType *arrayType, const glslang::TIntermediate *intermediate)
{
    return nullptr;
}

const ShaderReflection::DataType *ObjectRenderResourceTemplate::ParseFundamentalType(const glslang::TType *fundamentalType, const glslang::TIntermediate *intermediate)
{
    auto basicType = fundamentalType->getBasicType();
    if (fundamentalType->isMatrix())
    {
        int cols = fundamentalType->getMatrixCols();
        int rows = fundamentalType->getMatrixRows();
        if (cols == 3 && rows == 3 && basicType == glslang::EbtFloat)
        {
            return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Mat3);
        }
        else if (cols == 4 && rows == 4 && basicType == glslang::EbtFloat)
        {
            return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Mat4);
        }
    }
    else if (fundamentalType->isVector())
    {
        int size = fundamentalType->getVectorSize();
        
        if (basicType == glslang::EbtFloat)
        {
            if (size == 2) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Float2);
            if (size == 3) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Float3);
            if (size == 4) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Float4);
        }
        else if (basicType == glslang::EbtInt)
        {
            if (size == 2) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Int2);
            if (size == 3) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Int3);
            if (size == 4) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Int4);
        }
        else if (basicType == glslang::EbtUint)
        {
            if (size == 2) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::UInt2);
            if (size == 3) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::UInt3);
            if (size == 4) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::UInt4);
        }
    }
    else 
    {
        if (basicType == glslang::EbtFloat)
        {
            return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Float);
        }
        else if (basicType == glslang::EbtInt)
        {
            return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Int);
        }
        else if (basicType == glslang::EbtUint)
        {
            return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::UInt);
        }
    }
    Log::Error("Unknown Fundamental Type!");
    return nullptr;
}

const ShaderReflection::DataType *ObjectRenderResourceTemplate::ParseStructType(const glslang::TType *structType, const glslang::TIntermediate *intermediate)
{
    auto structName = structType->getTypeName().c_str();
    if (!customDataTypes.contains(structName))
    {
        const glslang::TTypeList* structMembers = structType->getStruct();
        Scope<ShaderReflection::Struct> type = CreateScope<ShaderReflection::Struct>(structName);
        uint32_t structSize = 0;
        for (uint32_t i = 0; i < structMembers->size(); i++)
        {
            const auto &memberLoc = (*structMembers)[i];
            auto memberType = memberLoc.type;

            std::string fieldName = memberType->getFieldName().c_str();

            auto fieldType = ParseDataType(memberType, intermediate);
            auto field = std::make_shared<ShaderReflection::Field>(fieldName, type.get(), fieldType);

            type->AddField(field);
            
        }

        customDataTypes[structName] = std::move(type);
        return customDataTypes[structName].get();
    }
    else 
    {
        return customDataTypes[structName].get();
    }
}

void ObjectRenderResourceTemplate::TraverseAST(const TIntermNode *node, const glslang::TIntermediate *intermediate)
{
    if (!node) return;

    if (auto* symbolNode = node->getAsSymbolNode()) 
    {
        auto resourceDesc = ParseResourceType(symbolNode, intermediate);
        auto paramName = symbolNode->getName().c_str();
        parameterTypes[paramName] = resourceDesc.type;
        bindings[paramName] = resourceDesc.binding;
    }

    // 递归遍历子节点（如果有）- 完全复制成功项目的逻辑
    if (auto aggregate = node->getAsAggregate(); aggregate != nullptr)
    {
        for (auto* child : aggregate->getSequence()) 
        {
            TraverseAST(child, intermediate);
        }
    }
}

void ObjectRenderResourceTemplate::ParseShaderSchema(const std::string &source)
{
    // 尝试使用片段着色器类型，因为它更通用
    glslang::TShader shader(EShLangVertex);
    const char* strings[] = { source.c_str() };
    shader.setStrings(strings, 1);
    shader.setEnvInput(glslang::EShSourceGlsl, EShLangFragment,
                      glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
    shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_5);

    if (!shader.parse(GetDefaultResources(), 450, false, EShMsgDefault))
    {
        Log::Error("Glslang Failed to Parse Shader! {}", shader.getInfoLog());
        return;  // 提前返回，避免访问可能损坏的intermediate
    }

    const glslang::TIntermediate* intermediate = shader.getIntermediate();
    if (!intermediate) {
        Log::Error("No intermediate representation found!");
        return;
    }

    const auto root = intermediate->getTreeRoot();
    if (!root) {
        Log::Error("No tree root found!");
        return;
    }

    TraverseAST(root, intermediate);
}

RenderResourceParameterPath ObjectRenderResourceTemplate::ParseParameterPath(const std::string &path) const
{
    RenderResourceParameterPath result;
    uint32_t i = 0;

    auto readIdent = [&]() {
        uint32_t start = i;
        while (i < path.size() && 
              (isalnum(path[i]) || path[i] == '_'))
            i++;
        return path.substr(start, i - start);
    };

    while (i < path.size()) {
        if (isalpha(path[i]) || path[i] == '_') {
            std::string ident = readIdent();
            result.elems.push_back({ 
                .type = RenderResourceParameterPath::PathElemType::Indent, 
                .name = ident 
            });
        } 
        else if (path[i] == '.') {
            i++;
        }
        else if (path[i] == '[') {
            i++;
            uint32_t start = i;
            while (isdigit(path[i])) i++;
            uint32_t index = std::stoi(path.substr(start, i - start));
            result.elems.push_back({ 
                .type = RenderResourceParameterPath::PathElemType::Index,
                .index = index 
            });

            if (path[i] == ']') i++;
        }
        else {
            Log::Error("Invalid character in path!");
        }
    }
    return result;
}

const ShaderReflection::DataType *ObjectRenderResourceTemplate::GetUniformParameterType(const RenderResourceParameterPath &path) const 
{
    auto uniformBlock = parameterTypes.at(path.elems[0].name)->As<ShaderReflection::UniformBlock>();
    const ShaderReflection::DataType *curType = uniformBlock->GetLayout().dataType;
    for (uint32_t i = 1; i < path.elems.size(); i++)
    {
        if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
        {
            return curType;
        }
        auto elem = path.elems[i];
        if (elem.type == RenderResourceParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
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
        else if (elem.type == RenderResourceParameterPath::PathElemType::Index && curType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
        {
            // TODO: deal with array;
            Log::Error("TODO array");
            return nullptr;
        }
    }
    return curType;
}

}