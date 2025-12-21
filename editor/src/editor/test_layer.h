#pragma once
#include "core/application/layer.h"
#include "editor/editorGUI/editor_window.h"
#include "editor/render_pass/geometry_pass.h"
#include "editor/render_pass/shading_pass.h"
#include "function/render/global_render_resource.h"
#include "function/render/object_render_resource.h"

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
    FrameData PrepareFrameData(Observer<EditorCamera> camera);

    Scope<EditorWindow> window;

    Scope<GlobalRenderResource> globalRenderResource;
    Scope<ObjectRenderResource> objectRenderResource;
    Scope<GeometryPass> geometryPass;
    Scope<ShadingPass> shadingPass;
    Scope<EditorCamera> editorCamera;
};

}