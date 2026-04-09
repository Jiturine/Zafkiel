#pragma once
#include "Core/Application/Layer.h"
#include "editor/EditorGUI/EditorWindow.h"
#include "editor/RenderPass/GeometryPass.h"
#include "editor/RenderPass/ShadingPass.h"
#include "editor/RenderPass/ShadowPass.h"
#include "editor/RenderPass/PostProcessingPass.h"
#include "editor/RenderPass/UIPass.h"

namespace Zafkiel 
{

class TestLayer : public Layer
{
  public:
    TestLayer() : Layer("Test Layer") {}
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float timestep) override;
  private:
    FrameData PrepareFrameData(EditorCamera &camera);

    Ref<UniformBufferContent> globalUniformBufferContent;
    Ref<RHIBuffer> globalUniformBuffer;

    Ref<DynamicUniformBufferContent> objectUniformBufferContent;
    Ref<RHIBuffer> objectUniformBuffer;

    Scope<EditorWindow> window;

    Scope<GeometryPass> geometryPass;
    Scope<ShadingPass> shadingPass;
    Scope<ShadowPass> shadowPass;
    Scope<PostProcessingPass> postProcessingPass;
    Scope<UIPass> uiPass;
    Scope<EditorCamera> editorCamera;

    static constexpr uint32 maxObjectSize = 500;
};

}