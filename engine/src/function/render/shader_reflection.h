#pragma once

namespace Zafkiel
{
namespace ShaderReflection
{

enum class ResourceTypeCategory
{
    Unknown,
    UniformBlock,
    SampledImage,
};

enum class DataTypeCategory
{
    Unknown,
    Fundamental,
    Struct,
    Array,
};

enum class LayoutRule 
{
    Unknown,
    Std140,
    Std430,
};

enum class SamplerType
{
    Texture2D,
};

class DataType
{
  public:
    virtual ~DataType() = default;

    DataType() = default;
    DataType(const std::string &name, DataTypeCategory category) 
        : name(name), category(category) {}

    template <typename T>
        requires std::derived_from<T, DataType>
    const T *As() const { return static_cast<const T *>(this); }

    std::string GetName() const { return name; }
    DataTypeCategory GetCategory() const { return category; }
    
  protected:
    std::string name;
    DataTypeCategory category;
};

class ResourceType
{
  public:
    virtual ~ResourceType() = default;

    ResourceType() = default;
    ResourceType(const std::string &name, ResourceTypeCategory category) 
        : name(name), category(category) {}

    template <typename T>
        requires std::derived_from<T, ResourceType>
    const T *As() const { return static_cast<const T *>(this); }

    std::string GetName() const { return name; }
    ResourceTypeCategory GetCategory() const { return category; }
    
  protected:
    std::string name;
    ResourceTypeCategory category;
};

enum class [[refl]] FundamentalKind
{
    None = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    UInt, UInt2, UInt3, UInt4,
    Bool
};

class Fundamental final : public DataType
{
  public:
    Fundamental() = default;
    Fundamental(const std::string &name, FundamentalKind kind)
        : DataType(name, DataTypeCategory::Fundamental), kind(kind) {}

    FundamentalKind GetKind() const { return kind; }
    uint32_t GetBytes() const;
    uint32_t GetBaseTypeCount() const;

  private:
    FundamentalKind kind;
};

inline static std::unordered_map<FundamentalKind, Fundamental> fundamentalTypeDict
{
    {FundamentalKind::Float, {"Float", FundamentalKind::Float}},
    {FundamentalKind::Float2, {"Float2", FundamentalKind::Float2}},
    {FundamentalKind::Float3, {"Float3", FundamentalKind::Float3}},
    {FundamentalKind::Float4, {"Float4", FundamentalKind::Float4}},
    {FundamentalKind::Mat3, {"Mat3", FundamentalKind::Mat3}},
    {FundamentalKind::Mat4, {"Mat4", FundamentalKind::Mat4}},
    {FundamentalKind::Int, {"Int", FundamentalKind::Int}},
    {FundamentalKind::Int2, {"Int2", FundamentalKind::Int2}},
    {FundamentalKind::Int3, {"Int3", FundamentalKind::Int3}},
    {FundamentalKind::Int4, {"Int4", FundamentalKind::Int4}},
    {FundamentalKind::UInt, {"UInt", FundamentalKind::UInt}},
    {FundamentalKind::UInt2, {"UInt", FundamentalKind::UInt2}},
    {FundamentalKind::UInt3, {"UInt", FundamentalKind::UInt3}},
    {FundamentalKind::UInt4, {"UInt", FundamentalKind::UInt4}},
    {FundamentalKind::Bool, {"UInt", FundamentalKind::Bool}},
};

inline const Fundamental *GetFundamentalType(FundamentalKind kind) { return &fundamentalTypeDict[kind]; }

class Array final : public DataType
{
  public:
    Array(const std::string &name, const DataType *type)
        : DataType(name, DataTypeCategory::Array) {}

    const DataType *GetElemType() { return elemType; }
    
  private:
    const DataType *elemType;
};


class Field;
class Struct final : public DataType
{ 
  public:
    Struct(const std::string &name) : DataType(name, DataTypeCategory::Struct) {}
    const std::vector<std::shared_ptr<Field>> &GetFields() const { return fields; }
    void AddField(const std::shared_ptr<Field> &field) { fields.push_back(field); }
    
  private:
    std::vector<std::shared_ptr<Field>> fields;
};

class Field
{
  public:
    Field(const std::string &name, const Struct *owner, const DataType *typeInfo)
        : name(name), owner(owner), info(typeInfo) {}

    const DataType *GetTypeInfo() const { return info; }

    const Struct *GetOwner() const { return owner; }

    std::string GetName() const { return name; }

  private:
    const Struct *owner;
    const DataType *info;
    std::string name;
};

class SampledImage final : public ResourceType
{
  public:
    SampledImage() = default;
    SampledImage(const std::string &name, SamplerType type) 
        : ResourceType(name, ResourceTypeCategory::SampledImage), samplerType(type) {}
    SamplerType GetSamplerType() const { return samplerType; }

  private:
    SamplerType samplerType;
};

inline static std::unordered_map<SamplerType, SampledImage> sampledImageDict
{
    {SamplerType::Texture2D, {"Texture2D", SamplerType::Texture2D}}
};
inline static const SampledImage *GetSampledImageType(SamplerType type) { return &sampledImageDict[type]; }

struct UniformFieldLayout 
{
    uint32_t offset;
    uint32_t size;

    uint32_t elemStride = 0; // 仅 Array 使用 
    uint32_t arraySize = 1;
    bool isArray = false;

    uint32_t matrixStride = 0; // Matrix
};

struct UniformValuePath 
{
    enum PathElemType { Indent, Index };
    struct PathElem 
    {
        PathElemType type;
        std::string name;
        uint32_t index;
    };
    std::vector<PathElem> elems;
};

using UniformBlockLayout = std::unordered_map<const DataType *, UniformFieldLayout>;

class UniformBlock final : public ResourceType
{ 
  public:
    UniformBlock(const std::string &name, LayoutRule layoutRule, const Struct *structType, UniformBlockLayout layout) 
        : ResourceType(name, ResourceTypeCategory::UniformBlock), layoutRule(layoutRule), structType(structType), layout(std::move(layout)) {}
  
    const Struct *GetType() const { return structType; }

    const UniformFieldLayout &GetFieldLayout(const DataType *dataType) const { return layout.at(dataType); }

    uint32_t GetSize() const { return layout.at(structType).size; }
    
  private:
    const Struct *structType;
    LayoutRule layoutRule;
    UniformBlockLayout layout;
};

}

using ShaderFundamentalType = ShaderReflection::FundamentalKind;

uint32_t GetSizeOfShaderFundamentalType(ShaderFundamentalType type);


}
  