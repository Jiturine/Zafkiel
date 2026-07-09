#pragma once
#include "Function/RHI/ShaderReflection.h"
#include "Core/Base/ImageFormat.h"

namespace Zafkiel 
{

class RHICommandList;

enum class RHIResourceType
{
    Buffer,
    Texture,
    GraphicsPipeline,
    VertexShader,
    FragmentShader,
    GeometryShader,
    Viewport,
};

class RHIResource : public RefCounted
{
  protected:
    RHIResource(RHIResourceType type) : resourceType(type) {}
    RHIResourceType resourceType;
};

enum class BufferUsageFlags
{
    None = 0,

    VertexBuffer = 1 << 0,

    IndexBuffer = 1 << 1,

    UniformBuffer = 1 << 2,

    Static = 1 << 3,

    Dynamic = 1 << 4,

    CPUAccessible = 1 << 5,
};
ENUM_CLASS_FLAGS(BufferUsageFlags)

struct VertexBufferElementLayout
{
    std::string name;
    uint32 location;
    uint32 size;
    uint32 offset;
    ShaderFundamentalType type;
};

struct VertexBufferLayout
{
    std::vector<VertexBufferElementLayout> elements;
    uint32 stride;
};

struct RHIBufferDesc
{
    uint32 size;
    BufferUsageFlags usages;
};

class RHIBuffer : public RHIResource
{
  public:
    RHIBuffer(const RHIBufferDesc &desc);
    
    virtual ~RHIBuffer() = default;

    uint32 GetSize() const { return desc.size; }
   
  protected:
    RHIBufferDesc desc;
};

enum class ImageUsageFlags
{
    None = 0,
    Upload = 1 << 0, // 上传图像数据
    Sampled = 1 << 1,
    ColorAttachment = 1 << 2,
    DepthAttachment = 1 << 3,
};
ENUM_CLASS_FLAGS(ImageUsageFlags)

enum class ImageLayout
{
    Undefined = 0,
    ShaderReadOnly,
    ColorAttachment,
    DepthAttachment,
    DepthStencilAttachment,
    PresentSrc
};

enum class TextureWrap
{
    None = 0,
    Repeat,
    Clamp
};

enum class TextureFilter
{
    None = 0,
    Nearest,
    Linear
};

struct RHITextureDesc
{
    uint32 width = 1;
    uint32 height = 1;
    ImageFormat format;
    ImageUsageFlags usages;
    ImageLayout initialLayout;
    TextureWrap wrap = TextureWrap::Repeat;
    TextureFilter filter = TextureFilter::Nearest;
    uint32 sampleCount = 1;
};

class RHITexture : public RHIResource
{
  public:
    virtual ~RHITexture() = default;

    RHITexture(const RHITextureDesc &desc)
        : RHIResource(RHIResourceType::Texture), desc(desc) {}

    ImageFormat GetFormat() const { return desc.format; }

    uint32 GetSampleCount() const { return desc.sampleCount; }

    uint32 GetWidth() const { return desc.width; }

    uint32 GetHeight() const { return desc.height; }

    const RHITextureDesc &GetDesc() const { return desc; } 
  
  protected:
    RHITextureDesc desc;
};

enum class ShaderType : uint8
{
    Vertex,
    Fragment,
    Geometry
};

namespace ShaderStage
{
enum Stage 
{
    Vertex = 0,
    Fragment = 1,
    Geometry = 2,
};
}

constexpr uint32 MaxShaderStages = 3;

struct ResourceTypeInfo
{
    std::string name;
    const ShaderReflection::ResourceType *type;
};

struct ShaderResourceTable
{
    uint32 bufferCount;
    uint32 imageCount;
    std::vector<ResourceTypeInfo> resourceTypeInfos;
    std::unordered_map<std::string, uint32> resourceNameToBinding;
    std::unordered_map<std::string, Scope<ShaderReflection::DataType>> customDataTypes;
    std::unordered_map<std::string, Scope<ShaderReflection::ResourceType>> customResourceTypes;
    std::vector<std::string> staticUniformBuffers;

    ShaderResourceTable(const ShaderResourceTable&) = delete;
    ShaderResourceTable& operator=(const ShaderResourceTable&) = delete;

    ShaderResourceTable() = default;
    ShaderResourceTable(ShaderResourceTable&&) = default;
    ShaderResourceTable& operator=(ShaderResourceTable&&) = default;

    ShaderReflection::ResourceType *GetResourceType(const std::string &name) 
    {
        return customResourceTypes.at(name).get();
    }
};

class RHIShader : public RHIResource
{
  public:
    explicit RHIShader(RHIResourceType resourceType, ShaderType shaderType, Scope<ShaderResourceTable> SRT)
        : RHIResource(resourceType), shaderType(shaderType), shaderResourceTable(MoveTemp(SRT)) {}

    virtual ~RHIShader() = default;

    ShaderType GetShaderType() const { return shaderType; }

    ShaderStage::Stage GetShaderStage() const 
    {
        if (shaderType == ShaderType::Vertex) return ShaderStage::Vertex;
        if (shaderType == ShaderType::Fragment) return ShaderStage::Fragment;
        return ShaderStage::Geometry;
    }

    ShaderResourceTable &GetResourceTable() { return *shaderResourceTable.get(); }

  protected:
    ShaderType shaderType;
    Scope<ShaderResourceTable> shaderResourceTable;
};

class RHIGraphicsShader : public RHIShader
{
  public:
    explicit RHIGraphicsShader(RHIResourceType resourceType, ShaderType shaderType, Scope<ShaderResourceTable> SRT)
        : RHIShader(resourceType, shaderType, MoveTemp(SRT)) {}
    
    virtual ~RHIGraphicsShader() = default;
};

struct ShaderVertexInput
{
    struct InputLayout
    {
        std::string name;
        uint32 location;
        uint32 size;
        uint32 offset;
        ShaderFundamentalType type;
    };
    std::vector<InputLayout> inputs;
    uint32 stride;
};

class RHIVertexShader : public RHIGraphicsShader
{
  public:
    RHIVertexShader(Scope<ShaderResourceTable> SRT)
        : RHIGraphicsShader(RHIResourceType::VertexShader, ShaderType::Vertex, MoveTemp(SRT)) {}
    
    virtual ~RHIVertexShader() = default;

    ShaderVertexInput &GetVertexInput() { return vertexInput; }

  protected:
    ShaderVertexInput vertexInput;
};

class RHIFragmentShader : public RHIGraphicsShader
{
  public:
    RHIFragmentShader(Scope<ShaderResourceTable> SRT) 
        : RHIGraphicsShader(RHIResourceType::FragmentShader, ShaderType::Fragment, MoveTemp(SRT)) {}
    
    virtual ~RHIFragmentShader() = default;
};

class RHIGeometryShader : public RHIGraphicsShader
{
  public:
    RHIGeometryShader(Scope<ShaderResourceTable> SRT) 
        : RHIGraphicsShader(RHIResourceType::GeometryShader, ShaderType::Geometry, MoveTemp(SRT)) {}
    
    virtual ~RHIGeometryShader() = default;
};

enum class PipelineType
{
    Graphics,
    Compute
};

enum class PrimitiveTopology
{
    None = 0,
    Points,
    Lines,
    Triangles,
    LineStrip,
    TriangleStrip,
    TriangleFan
};

enum class CullMode
{
    None = 0,
    Front,
    Back,
    FrontAndBack
};

enum class FrontFace
{
    CounterClockWise,
    ClockWise
};

enum class PolygonMode
{
    Fill,
    Wireframe
};

enum class BlendFunc
{
    None,
    Normal,
};

using ShaderKey = uint64;

ShaderKey CalcShaderKey(Buffer shaderCode);

class RHIRenderPassInfo;

constexpr uint32 MaxRenderTargets = 8;

struct RenderTargetDesc
{
    struct ColorAttachmentDesc
    {
        ImageFormat format;
        bool blendEnable = false;
        BlendFunc blendFunc = BlendFunc::None;
        uint32 sampleCount = 1;
    };

    struct DepthStencilAttachmentDesc
    {
        ImageFormat format;
        uint32 sampleCount = 1;
    };

    std::vector<ColorAttachmentDesc> colorAttachmentDescs;

    std::optional<DepthStencilAttachmentDesc> depthStencilAttachmentDesc;
};

struct RHIGraphicsPipelineDesc
{
    std::vector<RHIGraphicsShader *> shaders;
    PrimitiveTopology primitiveTopology;
    CullMode cullMode;
    FrontFace frontFace = FrontFace::CounterClockWise;
    PolygonMode polygonMode;
    bool depthTest;
    RenderTargetDesc renderTargetDesc;
};

class RHIGraphicsPipeline : public RHIResource
{
  public:
    RHIGraphicsPipeline(const RHIGraphicsPipelineDesc &desc) 
        : RHIResource(RHIResourceType::GraphicsPipeline), desc(desc) {}
    
    virtual ~RHIGraphicsPipeline() = default;

    virtual void SetDynamicOffsetIndex(ShaderStage::Stage stage, const std::string &name, uint32 index) = 0;

    virtual void SetUniformBuffer(ShaderStage::Stage stage, const std::string &name, RHIBuffer *buffer) = 0;

    virtual void SetTexture(ShaderStage::Stage stage, const std::string &name, RHITexture *texture) = 0;

    virtual void ClearResources() = 0;

    RHIGraphicsPipelineDesc &GetDesc() { return desc; }

    PrimitiveTopology GetPrimitiveTopology() const { return desc.primitiveTopology; }
    
    CullMode GetCullMode() const { return desc.cullMode; }

    FrontFace GetFrontFace() const { return desc.frontFace; }

    PolygonMode GetPolygonMode() const { return desc.polygonMode; }

    std::vector<RHIGraphicsShader *> &GetShaders() { return desc.shaders; }

    bool IsDepthTestOn() const { return desc.depthTest; }

  protected:
    RHIGraphicsPipelineDesc desc;
};

enum class AttachmentType 
{
    None = 0,
    Color,
    Depth,
    Stencil,
    DepthStencil,
    Swapchain
};

struct ClearValue
{
    union
    {
        float floatValue;
        vec2 vec2Value;
        vec3 vec3Value;
        vec4 vec4Value;
    };
    union
    {
        uint32 uintValue;
        uvec2 uvec2Value;
        uvec3 uvec3Value;
        uvec4 uvec4Value;
    };
};

struct RHIRenderPassInfo 
{
    struct ColorAttachmentInfo
    {
        RHITexture *texture;
        ClearValue clearValue;
        ImageLayout initialLayout;
        ImageLayout finalLayout;
    };

    std::vector<ColorAttachmentInfo> colorAttachments;

    struct DepthStencilAttachmentInfo
    {
        RHITexture *texture;
        ClearValue clearValue;
        ImageLayout initialLayout;
        ImageLayout finalLayout;
    };

    std::optional<DepthStencilAttachmentInfo> depthStencilAttachment;
};

struct UniformParameterPath
{
    enum PathElemType { Indent, Index };
    struct PathElem 
    {
        PathElemType type;
        std::string name;
        uint32_t index;
    };
    std::vector<PathElem> elems;

    static UniformParameterPath Parse(const std::string &path);
};

class UniformBufferContent : public RefCounted
{
  public:
    UniformBufferContent(const ShaderReflection::UniformBlock *uniformBlock);

    Buffer GetData() { return buffer; }

    void SetParameter(const std::string &parameterPath, Buffer data, ShaderFundamentalType type);
    
    ScopedBuffer GetParameter(const std::string parameterPath, ShaderFundamentalType type);

  private:
    const ShaderReflection::UniformBlock *uniformBlock;

    ScopedBuffer buffer;
};

class DynamicUniformBufferContent : public RefCounted
{
  public:
    DynamicUniformBufferContent(uint32 maxSize, const ShaderReflection::UniformBlock *uniformBlock, uint32 minUniformBufferOffsetAlignment);

    Buffer GetData() { return buffer; }

    void SetParameter(uint32 index, const std::string &parameterPath, Buffer data, ShaderFundamentalType type);

  private:
    const ShaderReflection::UniformBlock *uniformBlock;

    uint32 minUniformBufferOffsetAlignment;

    ScopedBuffer buffer;
    
};

class RHIViewport : public RHIResource
{
  public:
    RHIViewport() : RHIResource(RHIResourceType::Viewport) {}

    virtual void Resize(uint32 width, uint32 height) = 0;

    virtual RHITexture *GetBackendTexture() = 0;
};

}