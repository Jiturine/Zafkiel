#include "function/render/renderer.h"
#include "function/render/backends/opengl/opengl_context.h"
#include "function/render/backends/opengl/opengl_image.h"
#include "function/render/backends/vulkan/vulkan_context.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>

#include "resource/texture2d_asset.h"
#include "resource/material_asset.h"
#include "resource/mesh_asset.h"

#include "function/render/shader_reflection.h"

namespace Zafkiel 
{
Renderer::Renderer(GraphicsAPI API, const Window &window)
{
    Maths::SetAPI(API);
    Application::StartRenderThread([this, API, &window](){
        renderRegistryView = CreateScope<RenderRegistryView>(renderRegistry);
        if (API == GraphicsAPI::OpenGL)
        {
            graphicsContext = CreateScope<OpenGLContext>(window);
        }
        else if (API == GraphicsAPI::Vulkan)
        {
            graphicsContext = CreateScope<VulkanContext>(window);
        }
        InitImGui(window.GetHandle());
        glslang::InitializeProcess();
        LoadBuiltInMaterialTemplates();
    });
}

Renderer::~Renderer() 
{
    
}
void Renderer::Init(GraphicsAPI API, const Window &window)
{
    instance = new Renderer(API, window);
}

void Renderer::Destroy()
{
    Submit([](){
        instance->DestroyImGui();
        glslang::FinalizeProcess();
    });
    Application::KickRenderThread();
    Application::WaitRenderThread();
    Application::StopRenderThread();
    delete instance;
    instance = nullptr;
}

RenderHandle Renderer::CreateVertexBuffer(const float *vertices, uint32_t size)
{
    auto backend = graphicsContext->CreateVertexBufferBackend(vertices, size);
    RenderHandle handle;
    renderRegistry.vertexBuffers[handle] = CreateScope<VertexBuffer>(size, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateIndexBuffer(const uint32_t *indices, uint32_t count)
{
    auto backend = graphicsContext->CreateIndexBufferBackend(indices, count);
    RenderHandle handle;
    renderRegistry.indexBuffers[handle] = CreateScope<IndexBuffer>(count, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateUniformBuffer(uint32_t size)
{
    auto backend = graphicsContext->CreateUniformBufferBackend(size);
    RenderHandle handle;
    renderRegistry.uniformBuffers[handle] = CreateScope<UniformBuffer>(size, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateMesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices)
{
    auto vertexBuffer = CreateVertexBuffer(reinterpret_cast<const float *>(vertices.data()), sizeof(MeshVertex) * vertices.size());
    auto indexBuffer = CreateIndexBuffer(indices.data(), indices.size());
    RenderHandle handle;
    renderRegistry.meshes[handle] = CreateScope<Mesh>(vertexBuffer, indexBuffer);
    return handle;
}

RenderHandle Renderer::CreateGraphicsPipeline(const GraphicsPipelineSpecification &spec)
{
    auto shader = Borrow(renderRegistry.graphicsShaders.at(spec.shader));
    auto vertexModule = Borrow(renderRegistry.vertexModules.at(shader->GetVertexModule()));
    auto fragmentModule = Borrow(renderRegistry.fragmentModules.at(shader->GetFragmentModule()));
    auto renderPass = Borrow(renderRegistry.renderPasses.at(spec.renderPass));
    auto backend = graphicsContext->CreatePipelineBackend(spec, shader, vertexModule, fragmentModule, renderPass, Borrow(renderRegistryView));
    RenderHandle handle;
    renderRegistry.graphicsPipelines[handle] = CreateScope<GraphicsPipeline>(spec, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateVertexModule(Buffer code)
{
    auto backend = graphicsContext->CreateShaderModuleBackend(code, ShaderStage::Vertex);
    RenderHandle handle;
    renderRegistry.vertexModules[handle] = CreateScope<VertexModule>(code, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateFragmentModule(Buffer code)
{
    auto backend = graphicsContext->CreateShaderModuleBackend(code, ShaderStage::Fragment);
    RenderHandle handle;
    renderRegistry.fragmentModules[handle] = CreateScope<FragmentModule>(code, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateGraphicsShader(const Path &filePath)
{
    std::string source = FileSystem::ReadText(filePath);
    GlslPreprocessor preprocessor(source);
    auto shaderModuleSource = preprocessor.Preprocess();
    GlslCompiler compiler(filePath.parent_path());
    auto vertexCode = compiler.Compile(shaderModuleSource.vertexModuleSource, graphicsContext->GetAPI(), ShaderStage::Vertex,
        filePath.filename().stem().string() + "_vertex");
    auto vertexModule = CreateVertexModule(vertexCode);

    auto fragmentCode = compiler.Compile(shaderModuleSource.fragmentModuleSource, graphicsContext->GetAPI(), ShaderStage::Fragment,
        filePath.filename().stem().string() + "_fragment");
    auto fragmentModule = CreateFragmentModule(fragmentCode);

    // Renderer 层解析 handle 并传递对象
    auto vertexModuleObj = Borrow(renderRegistry.vertexModules.at(vertexModule));
    auto fragmentModuleObj = Borrow(renderRegistry.fragmentModules.at(fragmentModule));
    auto backend = graphicsContext->CreateShaderBackend(vertexModuleObj, fragmentModuleObj);

    RenderHandle handle;
    auto shader = CreateScope<GraphicsShader>(vertexModule, fragmentModule, std::move(backend));
    shader->CombineModules(vertexModuleObj);
    renderRegistry.graphicsShaders[handle] = std::move(shader);
    return handle;
}

RenderHandle Renderer::CreateImage(const ImageSpecification &spec)
{
    auto backend = graphicsContext->CreateImageBackend(spec);
    RenderHandle handle;
    renderRegistry.images[handle] = CreateScope<Image>(spec, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateTexture2D(const Texture2DSpecification &spec)
{
    ImageSpecification imageSpec
    {
        .format = spec.format,
        .usages = spec.usages,
        .updateFrequency = spec.updateFrequency,
        .width = spec.width,
        .height = spec.height,
        .samples = spec.samples
    };
    auto imageHandle = CreateImage(imageSpec);
    auto imageObj = Borrow(renderRegistry.images.at(imageHandle));
    auto imageBackend = imageObj->GetBackend();

    auto backend = graphicsContext->CreateTexture2DBackend(spec, imageBackend);

    RenderHandle handle;
    renderRegistry.texture2Ds[handle] = CreateScope<Texture2D>(spec, imageHandle, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateTexture2D(const Texture2DSpecification &spec, Buffer buffer)
{
    ImageSpecification imageSpec
    {
        .format = spec.format,
        .usages = spec.usages,
        .updateFrequency = spec.updateFrequency,
        .width = spec.width,
        .height = spec.height,
        .samples = spec.samples
    };
    if (std::find(imageSpec.usages.begin(), imageSpec.usages.end(), ImageUsage::Upload) == imageSpec.usages.end())
    {
        imageSpec.usages.push_back(ImageUsage::Upload);
    }
    auto imageHandle = CreateImage(imageSpec);
    auto imageObj = Borrow(renderRegistry.images.at(imageHandle));
    auto imageBackend = imageObj->GetBackend();

    auto backend = graphicsContext->CreateTexture2DBackend(spec, imageBackend);

    RenderHandle handle;
    renderRegistry.texture2Ds[handle] = CreateScope<Texture2D>(spec, imageHandle, std::move(backend));

    graphicsContext->SetImageData(imageObj, buffer);

    return handle;
}

RenderHandle Renderer::CreateRenderPass(const RenderPassSpecification &spec)
{
    auto backend = graphicsContext->CreateRenderPassBackend(spec);

    RenderHandle handle;
    renderRegistry.renderPasses[handle] = CreateScope<RenderPass>(spec, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateFrameBuffer(const FrameBufferSpecification &spec)
{
    // Renderer 层负责解析 RenderHandle 到实际对象
    auto renderPass = Borrow(renderRegistry.renderPasses.at(spec.renderPass));

    std::vector<Borrow<Image>> images;
    images.reserve(spec.attachments.size());
    for (auto &attachmentHandle : spec.attachments) 
    {
        images.push_back(Borrow(renderRegistry.images.at(attachmentHandle)));
    }

    auto backend = graphicsContext->CreateFrameBufferBackend(spec, renderPass, images);

    RenderHandle handle;
    renderRegistry.frameBuffers[handle] = CreateScope<FrameBuffer>(spec, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateShaderMaterial(RenderHandle shaderMaterialTemplateHandle)
{
    auto shaderMaterialTemplate = Borrow(renderRegistry.shaderMaterialTemplates.at(shaderMaterialTemplateHandle));
    auto schema = Borrow(renderRegistry.shaderMaterialSchemas.at(shaderMaterialTemplate->GetSchema()));
    auto backend = graphicsContext->CreateShaderMaterialBackend(shaderMaterialTemplate->GetBackend(), schema);

    std::vector<std::optional<ShaderResource>> resources(schema->GetResourceTypeInfos().size());
    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schema->GetResourceTypeInfos()))
    {
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            resources[binding] = ShaderResource(resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>());
        }
        else if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            // TODO: Change this: defalut val
            resources[binding] = ShaderResource(resourceTypeInfo.value().type->As<ShaderReflection::SampledImage>()->GetSamplerType());
        }
    }

    RenderHandle handle;
    renderRegistry.shaderMaterials[handle] = CreateScope<ShaderMaterial>(shaderMaterialTemplate->GetSchema(), std::move(resources), std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateShaderMaterialTemplate(RenderHandle shaderMaterialSchema)
{
    auto schema = Borrow(renderRegistry.shaderMaterialSchemas.at(shaderMaterialSchema));
    auto backend = graphicsContext->CreateShaderMaterialTemplateBackend(schema);
    RenderHandle handle;
    renderRegistry.shaderMaterialTemplates[handle] = CreateScope<ShaderMaterialTemplate>(shaderMaterialSchema, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateShaderMaterialSchema(const Path &path)
{
    RenderHandle handle;
    renderRegistry.shaderMaterialSchemas[handle] = CreateScope<ShaderMaterialSchema>(path);
    return handle;
}

RenderHandle Renderer::CreateGlobalMaterial(const Path &path)
{
    RenderHandle handle;
    auto schema = CreateShaderMaterialSchema(path);
    auto shaderMaterialTemplate = CreateShaderMaterialTemplate(schema);
    auto shaderMaterial = CreateShaderMaterial(shaderMaterialTemplate);
    renderRegistry.globalMaterials[handle] = CreateScope<GlobalMaterial>(schema, shaderMaterialTemplate, shaderMaterial);
    return handle;
}

RenderHandle Renderer::CreatePassMaterial(const Path &path)
{
    RenderHandle handle;
    auto schema = CreateShaderMaterialSchema(path);
    auto shaderMaterialTemplate = CreateShaderMaterialTemplate(schema);
    auto shaderMaterial = CreateShaderMaterial(shaderMaterialTemplate);
    renderRegistry.passMaterials[handle] = CreateScope<PassMaterial>(schema, shaderMaterialTemplate, shaderMaterial);
    return handle;
}

RenderHandle Renderer::CreateObjectShaderMaterial(const Path &path)
{
    auto schema = CreateShaderMaterialSchema(path);
    auto objectShaderMaterialTemplate = CreateObjectShaderMaterialTemplate(schema);
    auto objectShaderMaterialTemplateObj = Borrow(renderRegistry.objectShaderMaterialTemplates.at(objectShaderMaterialTemplate));
    auto schemaObj = Borrow(renderRegistry.shaderMaterialSchemas.at(schema));
    auto backend = graphicsContext->CreateObjectShaderMaterialBackend(objectShaderMaterialTemplateObj->GetBackend(), schemaObj);
    
    std::vector<std::optional<ObjectShaderResource>> resources(schemaObj->GetResourceTypeInfos().size());
    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schemaObj->GetResourceTypeInfos()))
    {
        if (!resourceTypeInfo) continue;
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            resources[binding] = ObjectShaderResource(resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>());
        }
    }

    RenderHandle handle;
    renderRegistry.objectShaderMaterials[handle] = CreateScope<ObjectShaderMaterial>(schema, objectShaderMaterialTemplate, std::move(resources), std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateObjectShaderMaterialTemplate(RenderHandle schema)
{
    auto schemaObj = Borrow(renderRegistry.shaderMaterialSchemas.at(schema));
    auto backend = graphicsContext->CreateObjectShaderMaterialTemplateBackend(schemaObj);
    RenderHandle handle;
    renderRegistry.objectShaderMaterialTemplates[handle] = CreateScope<ObjectShaderMaterialTemplate>(schema, std::move(backend));
    return handle;
}

RenderHandle Renderer::CreateSurfaceMaterial(const SurfaceMaterialSpecification &spec, Ref<MaterialAsset> asset)
{
    auto shaderMaterial = CreateShaderMaterial(spec.shaderMaterialTemplate);
    auto shaderMaterialObj = MutBorrow(renderRegistry.shaderMaterials.at(shaderMaterial));
    RenderHandle handle;
    
    // 根据 MaterialAsset 赋初值
    for (auto [binding, resource] : std::views::enumerate(shaderMaterialObj->GetResources()))
    {
        if (!resource) continue;
        if (resource.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            resource = ShaderResource(resource.value().type->As<ShaderReflection::UniformBlock>());
            resource.value().uniformBuffer = asset->GetResources().at(binding).value().uniformBuffer.Clone();
        }
        else if (resource.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            if (!asset->GetResources().at(binding)) continue;
            auto texture2DAsset = AssetManager::GetAsset(asset->GetResources().at(binding).value().handle).As<Texture2DAsset>();
            resource = ShaderResource(ShaderReflection::SamplerType::Texture2D, Renderer::Instance().GetTexture2DFromAsset(texture2DAsset));
            // TODO: 优化
            shaderMaterialObj->GetBackend()->SetTexture2D(binding, resource.value().handle.value());
        }
    }
    
    renderRegistry.surfaceMaterials[handle] = CreateScope<SurfaceMaterial>(spec, shaderMaterial);
    return handle;
}

RenderHandle Renderer::GetTexture2DFromShaderMaterial(RenderHandle shaderMaterial, const std::string &alias)
{
    auto shaderMaterialObj = Borrow(renderRegistry.shaderMaterials.at(shaderMaterial));
    auto schemaHandle = shaderMaterialObj->GetSchema();
    auto schema = Borrow(renderRegistry.shaderMaterialSchemas.at(schemaHandle));
    auto texture2DName = schema->GetAliasFullPath(alias);
    auto binding = schema->GetResourceBinding(texture2DName);
    return shaderMaterialObj->GetResources().at(binding).value().handle.value(); 
}

void Renderer::SetTexture2DFromShaderMaterial(RenderHandle shaderMaterial, const std::string &alias, RenderHandle texture2D)
{
    auto shaderMaterialObj = MutBorrow(renderRegistry.shaderMaterials.at(shaderMaterial));
    auto schemaHandle = shaderMaterialObj->GetSchema();
    auto schema = Borrow(renderRegistry.shaderMaterialSchemas.at(schemaHandle));
    auto texture2DName = schema->GetAliasFullPath(alias);
    auto binding = schema->GetResourceBinding(texture2DName);
    shaderMaterialObj->SetTexture2D(binding, texture2D);
}

void Renderer::SetTexture2DFromGlobalMaterial(RenderHandle globalMaterial, const std::string &alias, RenderHandle texture2D)
{
    auto material = Borrow(renderRegistry.globalMaterials.at(globalMaterial));
    SetTexture2DFromShaderMaterial(material->GetShaderMaterial(), alias, texture2D);
}
void Renderer::SetTexture2DFromPassMaterial(RenderHandle passMaterial, const std::string &alias, RenderHandle texture2D)
{
    auto material = Borrow(renderRegistry.passMaterials.at(passMaterial));
    SetTexture2DFromShaderMaterial(material->GetShaderMaterial(), alias, texture2D);
}
void Renderer::SetTexture2DFromSurfaceMaterial(RenderHandle surfaceMaterial, const std::string &alias, RenderHandle texture2D)
{
    auto material = Borrow(renderRegistry.surfaceMaterials.at(surfaceMaterial));
    SetTexture2DFromShaderMaterial(material->GetShaderMaterial(), alias, texture2D);
}

ScopedBuffer Renderer::GetUniformFromShaderMaterial(RenderHandle shaderMaterial, const std::string &alias, ShaderFundamentalType type)
{
    auto shaderMaterialObj = Borrow(renderRegistry.shaderMaterials.at(shaderMaterial));
    auto schemaHandle = shaderMaterialObj->GetSchema();
    auto schema = Borrow(renderRegistry.shaderMaterialSchemas.at(schemaHandle));
    auto paramPath = schema->GetAliasFullPath(alias);
    auto parsedPath = ParseParameterPath(paramPath);

    auto binding = schema->GetResourceBinding(parsedPath.elems[0].name);
    auto uniformBlock = schema->GetResourceTypeInfos().at(binding).value().type->As<ShaderReflection::UniformBlock>();
    auto uniformBufferData = shaderMaterialObj->GetResources()[binding].value().uniformBuffer.Data<uint8_t>();
    const ShaderReflection::DataType *curType = uniformBlock->GetType();
    for (uint32_t i = 1; i < parsedPath.elems.size(); i++)
    {
        auto elem = parsedPath.elems[i];
        if (elem.type == ShaderMaterialParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curType->As<ShaderReflection::Struct>()->GetFields();
            auto it = std::find_if(fields.begin(), fields.end(), [&](const auto &field) { return field->GetName() == elem.name; });
            if (it == fields.end())
            {
                Log::Error("Invalid Parameter Path!");
                return {};
            }
            curType = (*it)->GetTypeInfo();
        }
        else if (elem.type == ShaderMaterialParameterPath::PathElemType::Index && curType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
        {
            // TODO: deal with array;
            Log::Error("TODO array");
            return {};
        }
    }
    if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        auto &fieldLayout = uniformBlock->GetFieldLayout(curType);
        auto kind = curType->As<ShaderReflection::Fundamental>()->GetKind();
        switch (kind)
        {
            using enum ShaderFundamentalType;
        case Float: case Float2: case Float3: case Float4:
        case Int: case Int2: case Int3: case Int4:
        case UInt: case UInt2: case UInt3: case UInt4:
        case Bool:
            return ScopedBuffer(uniformBufferData + fieldLayout.offset, GetSizeOfShaderFundamentalType(kind));
        case Mat3:
            {
                auto src = uniformBufferData + fieldLayout.offset;
                mat3 res;
                for (int col = 0; col < 3; ++col)
                {
                    memcpy(
                        &res[col][0], 
                        src + col * fieldLayout.matrixStride,
                        sizeof(float) * 3
                    );
                }
                return ScopedBuffer((uint8_t *)&res, fieldLayout.matrixStride * 3);
            }
        case Mat4:
            {
                auto src = uniformBufferData + fieldLayout.offset;
                mat4 res;
                for (int col = 0; col < 4; ++col)
                {
                    memcpy(
                        &res[col][0], 
                        src + col * fieldLayout.matrixStride,
                        sizeof(float) * 4
                    );
                }
                return ScopedBuffer((uint8_t *)&res, fieldLayout.matrixStride * 4);
            }
        default:
            Log::Error("Renderer::GetUniformFromShaderMaterial Unknown Uniform Param Type!");
            return {};
        }
    }
    else 
    {
        Log::Error("Invalid Parameter Path!");
        return {};
    }
}

void Renderer::SetUniformFromShaderMaterial(RenderHandle shaderMaterial, const std::string &alias, ShaderFundamentalType type, Buffer data)
{
    auto shaderMaterialObj = MutBorrow(renderRegistry.shaderMaterials.at(shaderMaterial));
    auto schemaHandle = shaderMaterialObj->GetSchema();
    auto schema = Borrow(renderRegistry.shaderMaterialSchemas.at(schemaHandle));
    auto paramPath = schema->GetAliasFullPath(alias);
    auto parsedPath = ParseParameterPath(paramPath);

    auto binding = schema->GetResourceBinding(parsedPath.elems[0].name);
    auto uniformBlock = schema->GetResourceTypeInfos().at(binding).value().type->As<ShaderReflection::UniformBlock>();
    auto uniformBufferData = shaderMaterialObj->GetResources()[binding].value().uniformBuffer.Data<uint8_t>();
    const ShaderReflection::DataType *curType = uniformBlock->GetType();
    for (uint32_t i = 1; i < parsedPath.elems.size(); i++)
    {
        auto elem = parsedPath.elems[i];
        if (elem.type == ShaderMaterialParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curType->As<ShaderReflection::Struct>()->GetFields();
            auto it = std::find_if(fields.begin(), fields.end(), [&](const auto &field) { return field->GetName() == elem.name; });
            if (it == fields.end())
            {
                Log::Error("Invalid Parameter Path!");
                return;
            }
            curType = (*it)->GetTypeInfo();
        }
        else if (elem.type == ShaderMaterialParameterPath::PathElemType::Index && curType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
        {
            // TODO: deal with array;
            Log::Error("TODO array");
            return;
        }
    }
    if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        auto &fieldLayout = uniformBlock->GetFieldLayout(curType);
        auto kind = curType->As<ShaderReflection::Fundamental>()->GetKind();
        switch (kind)
        {
            using enum ShaderFundamentalType;
        case Float: case Float2: case Float3: case Float4:
        case Int: case Int2: case Int3: case Int4:
        case UInt: case UInt2: case UInt3: case UInt4:
        case Bool:
            memcpy(uniformBufferData + fieldLayout.offset, data.Data<void>(), data.Size<uint8_t>());
            break;
        case Mat3:
            {
                mat3 matrix;
                memcpy(&matrix, data.Data<void>(), data.Size<uint8_t>());
                auto dst = uniformBufferData + fieldLayout.offset;
                for (int col = 0; col < 3; ++col)
                {
                    memcpy(
                        dst + col * fieldLayout.matrixStride, 
                        &matrix[col][0],
                        sizeof(float) * 3
                    );
                }
                break;
            }
        case Mat4:
            {
                mat4 matrix;
                memcpy(&matrix, data.Data<void>(), data.Size<uint8_t>());
                auto dst = uniformBufferData + fieldLayout.offset;
                for (int col = 0; col < 4; ++col)
                {
                    memcpy(
                        dst + col * fieldLayout.matrixStride, 
                        &matrix[col][0],
                        sizeof(float) * 4
                    );
                }
                break;
            }
        default:
            Log::Error("Renderer::SetUniformFromShaderMaterial Unknown Uniform Param Type!");
            return;
        }
    }
    else 
    {
        Log::Error("Invalid Parameter Path!");
    }
}

void Renderer::SetUniformFromGlobalMaterial(RenderHandle globalMaterial, const std::string &alias, ShaderFundamentalType type, Buffer data)
{
    auto material = Borrow(renderRegistry.globalMaterials.at(globalMaterial));
    SetUniformFromShaderMaterial(material->GetShaderMaterial(), alias, type, data);
}
void Renderer::SetUniformFromPassMaterial(RenderHandle passMaterial, const std::string &alias, ShaderFundamentalType type, Buffer data)
{
    auto material = Borrow(renderRegistry.passMaterials.at(passMaterial));
    SetUniformFromShaderMaterial(material->GetShaderMaterial(), alias, type, data);
}
void Renderer::SetUniformFromSurfaceMaterial(RenderHandle surfaceMaterial, const std::string &alias, ShaderFundamentalType type, Buffer data)
{
    auto material = Borrow(renderRegistry.surfaceMaterials.at(surfaceMaterial));
    SetUniformFromShaderMaterial(material->GetShaderMaterial(), alias, type, data);
}

void Renderer::SetUniformFromObjectShaderMaterial(RenderHandle objectShaderMaterial, uint32_t index, const std::string &alias, ShaderFundamentalType type, Buffer data)
{
    auto shaderMaterialObj = MutBorrow(renderRegistry.objectShaderMaterials.at(objectShaderMaterial));
    auto schemaHandle = shaderMaterialObj->GetSchema();
    auto schema = Borrow(renderRegistry.shaderMaterialSchemas.at(schemaHandle));
    auto paramPath = schema->GetAliasFullPath(alias);
    auto parsedPath = ParseParameterPath(paramPath);

    auto binding = schema->GetResourceBinding(parsedPath.elems[0].name);
    auto uniformBlock = schema->GetResourceTypeInfos().at(binding).value().type->As<ShaderReflection::UniformBlock>();
    auto uniformBufferData = shaderMaterialObj->GetResources()[binding].value().dynamicUniformBuffer.Data<uint8_t>() + index * uniformBlock->GetSize();
    const ShaderReflection::DataType *curType = uniformBlock->GetType();
    for (uint32_t i = 1; i < parsedPath.elems.size(); i++)
    {
        auto elem = parsedPath.elems[i];
        if (elem.type == ShaderMaterialParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curType->As<ShaderReflection::Struct>()->GetFields();
            auto it = std::find_if(fields.begin(), fields.end(), [&](const auto &field) { return field->GetName() == elem.name; });
            if (it == fields.end())
            {
                Log::Error("Invalid Parameter Path!");
                return;
            }
            curType = (*it)->GetTypeInfo();
        }
        else if (elem.type == ShaderMaterialParameterPath::PathElemType::Index && curType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
        {
            // TODO: deal with array;
            Log::Error("TODO array");
            return;
        }
    }
    if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        auto &fieldLayout = uniformBlock->GetFieldLayout(curType);
        auto kind = curType->As<ShaderReflection::Fundamental>()->GetKind();
        switch (kind)
        {
            using enum ShaderFundamentalType;
        case Float: case Float2: case Float3: case Float4:
        case Int: case Int2: case Int3: case Int4:
        case UInt: case UInt2: case UInt3: case UInt4:
        case Bool:
            memcpy(uniformBufferData + fieldLayout.offset, data.Data<void>(), data.Size<uint8_t>());
            break;
        case Mat3:
            {
                mat3 matrix;
                memcpy(&matrix, data.Data<void>(), data.Size<uint8_t>());
                auto dst = uniformBufferData + fieldLayout.offset;
                for (int col = 0; col < 3; ++col)
                {
                    memcpy(dst + col * fieldLayout.matrixStride, &matrix[col][0], sizeof(float) * 3);
                }
                break;
            }
        case Mat4:
            {
                mat4 matrix;
                memcpy(&matrix, data.Data<void>(), data.Size<uint8_t>());
                auto dst = uniformBufferData + fieldLayout.offset;
                for (int col = 0; col < 4; ++col)
                {
                    memcpy(dst + col * fieldLayout.matrixStride, &matrix[col][0], sizeof(float) * 4);
                }
                break;
            }
        default:
            Log::Error("Renderer::SetUniformFromObjectShaderMaterial Unknown Uniform Param Type!");
            return;
        }
    }
    else 
    {
        Log::Error("Invalid Parameter Path!");
    }
}

void Renderer::InvalidateImage(RenderHandle handle, const ImageSpecification &spec)
{
    graphicsContext->WaitIdle();
    auto image = Borrow(renderRegistry.images.at(handle));
    renderRegistry.images[handle] = CreateScope<Image>(spec, graphicsContext->CreateImageBackend(spec));
}

void Renderer::InvalidateTexture2D(RenderHandle handle, const Texture2DSpecification &spec)
{
    graphicsContext->WaitIdle();
    auto texture2D = Borrow(renderRegistry.texture2Ds.at(handle));
    ImageSpecification imageSpec
    {
        .format = spec.format,
        .usages = spec.usages,
        .updateFrequency = spec.updateFrequency,
        .width = spec.width,
        .height = spec.height,
        .samples = spec.samples
    };
    InvalidateImage(texture2D->GetImage(), imageSpec);
    auto imageBackend = renderRegistry.images.at(texture2D->GetImage())->GetBackend();
    renderRegistry.texture2Ds[handle] = CreateScope<Texture2D>(spec, texture2D->GetImage(), graphicsContext->CreateTexture2DBackend(spec, imageBackend));
}

void Renderer::InvalidateFrameBuffer(RenderHandle handle, const FrameBufferSpecification &spec)
{
    graphicsContext->WaitIdle();
    auto frameBuffer = Borrow(renderRegistry.frameBuffers.at(handle));

    auto renderPass = Borrow(renderRegistry.renderPasses.at(spec.renderPass));

    std::vector<Borrow<Image>> images;
    images.reserve(spec.attachments.size());
    for (auto &attachmentHandle : spec.attachments) 
    {
        images.push_back(Borrow(renderRegistry.images.at(attachmentHandle)));
    }

    auto frameBufferBackend = graphicsContext->CreateFrameBufferBackend(spec, renderPass, images);
    renderRegistry.frameBuffers[handle] = CreateScope<FrameBuffer>(spec, std::move(frameBufferBackend));
}

void Renderer::ResizeImage(RenderHandle handle, uint32_t width, uint32_t height)
{
    auto image = Borrow(renderRegistry.images.at(handle));
    auto spec = image->GetSpecification();
    spec.width = width;
    spec.height = height;
    InvalidateImage(handle, spec);
}
void Renderer::ResizeTexture2D(RenderHandle handle, uint32_t width, uint32_t height)
{
    auto texture2D = Borrow(renderRegistry.texture2Ds.at(handle));
    auto spec = texture2D->GetSpecification();
    spec.width = width;
    spec.height = height;
    InvalidateTexture2D(handle, spec);
}
void Renderer::ResizeFrameBuffer(RenderHandle handle, uint32_t width, uint32_t height)
{
    auto frameBuffer = Borrow(renderRegistry.frameBuffers.at(handle));
    auto spec = frameBuffer->GetSpecification();
    spec.width = width;
    spec.height = height;
    InvalidateFrameBuffer(handle, spec);
}

RenderHandle Renderer::GetMeshFromAsset(const Ref<MeshAsset> &meshAsset)
{
    if (!meshAssetMap.contains(meshAsset->handle))
    {
        auto meshHandle = CreateMesh(meshAsset->GetVertices(), meshAsset->GetIndices());
        meshAssetMap[meshAsset->handle] = meshHandle;
    }
    return meshAssetMap[meshAsset->handle];
}

RenderHandle Renderer::GetSurfaceMaterialFromAsset(const Ref<MaterialAsset> &materialAsset)
{
    if (!materialAssetMap.contains(materialAsset->handle))
    {
        SurfaceMaterialSpecification spec 
        {
            .name = materialAsset->GetName(),
            .shaderFamily = materialAsset->GetShaderFamily(),
            .shaderMaterialTemplate = builtInMaterialTemplates[materialAsset->GetShaderFamily()]
        };
        auto materialHandle = CreateSurfaceMaterial(spec, materialAsset);
        materialAssetMap[materialAsset->handle] = materialHandle;
    }
    return materialAssetMap.at(materialAsset->handle);
}

RenderHandle Renderer::GetTexture2DFromAsset(const Ref<Texture2DAsset> &texture2DAsset)
{
    if (!texture2DAssetMap.contains(texture2DAsset->handle))
    {
        texture2DAssetMap[texture2DAsset->handle] = CreateTexture2D(texture2DAsset->GetSpecification(), texture2DAsset->GetBuffer());
    }
    return texture2DAssetMap.at(texture2DAsset->handle);
}

void Renderer::LoadBuiltInMaterialTemplates()
{
    builtInMaterialSchemas[ShaderFamily::BlinnPhong] = CreateShaderMaterialSchema("assets/shaders/schema/blinn_phong.zss");
    builtInMaterialTemplates[ShaderFamily::BlinnPhong] = CreateShaderMaterialTemplate(builtInMaterialSchemas[ShaderFamily::BlinnPhong]);
}

void Renderer::InitImGui(SDL_Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;

    ImGui::StyleColorsDark();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    io.Fonts->AddFontFromFileTTF("assets/fonts/HarmonyOS_Sans_SC/HarmonyOS_Sans_SC_Regular.ttf");

    if (graphicsContext->GetAPI() == GraphicsAPI::OpenGL)
    {
        ImGui_ImplSDL3_InitForOpenGL(window, graphicsContext.As<OpenGLContext>().GetHandle());
        ImGui_ImplOpenGL3_Init("#version 450");
    }
    else 
    {

        ImGui_ImplSDL3_InitForVulkan(window);
        ImGui_ImplVulkan_InitInfo init_info = {};
        auto &context = graphicsContext.As<VulkanContext>();
        auto mainRenderPassBackend = context.GetMainRenderPass()->GetBackend().As<VulkanRenderPassBackend>();
        init_info.ApiVersion = VK_API_VERSION_1_4;
        init_info.Instance = *context.GetInstance();
        init_info.PhysicalDevice = *context.GetPhysicalDevice()->GetHandle();
        init_info.Device = *context.GetDevice()->GetHandle();
        init_info.QueueFamily = context.GetPhysicalDevice()->GetGraphicsQueueIndex();
        init_info.Queue = *context.GetDevice()->GetGraphicsQueue();
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = *context.GetDescriptorManager()->GetDescriptorPool();
        init_info.PipelineInfoMain.RenderPass = *mainRenderPassBackend->GetHandle();
        init_info.PipelineInfoMain.Subpass = 0;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&init_info);
    }
}

void Renderer::DestroyImGui() 
{
    if (graphicsContext->GetAPI() == GraphicsAPI::OpenGL)
        ImGui_ImplOpenGL3_Shutdown();
    else if (graphicsContext->GetAPI() == GraphicsAPI::Vulkan)
    {
        graphicsContext.As<VulkanContext>().GetDevice()->GetHandle().waitIdle();
        ImGui_ImplVulkan_Shutdown();
    }
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

std::vector<ImTextureRef> Renderer::RegisterImGuiTexture(RenderHandle texture)
{
    auto textureObj = Borrow(renderRegistry.texture2Ds.at(texture));
    if (graphicsContext->GetAPI() == GraphicsAPI::Vulkan)
    {
        auto texture2DBackend = textureObj->GetBackend().As<VulkanTexture2DBackend>();
        auto imageBackend = renderRegistry.images.at(textureObj->GetImage())->GetBackend().As<VulkanImageBackend>();
        auto descriptorSets = graphicsContext.As<VulkanContext>().RegisterImGuiTexture2D(imageBackend, texture2DBackend);
        std::vector<ImTextureRef> result;
        for (auto &descriptorSet : descriptorSets)
        {
            result.push_back((ImTextureRef)descriptorSet);
        }
        return result;
    }
    else if (graphicsContext->GetAPI() == GraphicsAPI::OpenGL)
    {
        auto imageBackend =  Borrow(renderRegistry.images.at(textureObj->GetImage()))->GetBackend().As<OpenGLImageBackend>();
        return { imageBackend->GetRendererID() };
    }
    else return {};
}

void Renderer::UnregisterImGuiTexture(RenderHandle texture)
{
    if (graphicsContext->GetAPI() == GraphicsAPI::Vulkan)
    {
        auto textureObj = Borrow(renderRegistry.texture2Ds.at(texture));
        auto image = Borrow(renderRegistry.images.at(textureObj->GetImage()));
        graphicsContext.As<VulkanContext>().UnregisterImGuiTexture2D(image->GetBackend().As<VulkanImageBackend>());
    }
}

void Renderer::CmdBindGlobalMaterial(RenderHandle globalMaterial)
{
    auto material = Borrow(renderRegistry.globalMaterials.at(globalMaterial));
    auto shaderMaterial = Borrow(renderRegistry.shaderMaterials.at(material->GetShaderMaterial()));
    
    graphicsContext->BindGlobalMaterial(shaderMaterial, Borrow(renderRegistryView));
}
void Renderer::CmdBindPassMaterial(RenderHandle passMaterial)
{
    auto material = Borrow(renderRegistry.passMaterials.at(passMaterial));
    auto shaderMaterial = Borrow(renderRegistry.shaderMaterials.at(material->GetShaderMaterial()));
    graphicsContext->BindPassMaterial(shaderMaterial, Borrow(renderRegistryView));
}

void Renderer::CmdBindSurfaceMaterial(RenderHandle surfaceMaterial)
{
    auto material = Borrow(renderRegistry.surfaceMaterials.at(surfaceMaterial));
    auto shaderMaterial = Borrow(renderRegistry.shaderMaterials.at(material->GetShaderMaterial()));
    graphicsContext->BindSurfaceMaterial(shaderMaterial, Borrow(renderRegistryView));
}
void Renderer::CmdBindObjectShaderMaterial(uint32_t index, RenderHandle objectShaderMaterial)
{
    auto material = Borrow(renderRegistry.objectShaderMaterials.at(objectShaderMaterial));
    graphicsContext->BindObjectShaderMaterial(index, material, Borrow(renderRegistryView));
}
void Renderer::CmdDrawIndexed(RenderHandle vertexBuffer, RenderHandle indexBuffer)
{
    auto vertexBufferObj = Borrow(renderRegistry.vertexBuffers.at(vertexBuffer));
    auto indexBufferObj = Borrow(renderRegistry.indexBuffers.at(indexBuffer));
    graphicsContext->DrawIndexed(vertexBufferObj, indexBufferObj);
}

void Renderer::CmdBeginRenderPass(const RenderPassBeginInfo &beginInfo)
{
    auto renderPass = Borrow(renderRegistry.renderPasses.at(beginInfo.renderPass));
    auto frameBuffer = Borrow(renderRegistry.frameBuffers.at(beginInfo.frameBuffer));
    graphicsContext->BeginRenderPass(renderPass, frameBuffer, beginInfo.clearValues);
}

void Renderer::CmdEndRenderPass()
{
    graphicsContext->EndRenderPass();
}

void Renderer::CmdBindGraphicsPipeline(RenderHandle pipeline)
{
    auto graphicsPipeline = Borrow(renderRegistry.graphicsPipelines.at(pipeline));
    graphicsContext->BindGraphicsPipeline(graphicsPipeline, Borrow(renderRegistryView));
}

void Renderer::UploadObjectShaderMaterialUniform(RenderHandle objectShaderMaterial)
{
    auto material = Borrow(renderRegistry.objectShaderMaterials.at(objectShaderMaterial));
    graphicsContext->UploadObjectShaderMaterialUniform(material->GetBackend(), material->GetResources());
}

void Renderer::BeginFrame()
{
    graphicsContext->BeginFrame();
}

void Renderer::EndFrame()
{
    graphicsContext->EndFrame();
}

}