#pragma once
#include "core/application/layer.h"
#include "editor/editorGUI/editor_window.h"
#include "editor/render_pass/geometry_pass.h"
#include "editor/render_pass/shading_pass.h"
#include "editor/render_pass/shadow_pass.h"
#include "function/render/global_material.h"
#include "function/render/object_shader_material.h"

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
    FrameData PrepareFrameData(Borrow<EditorCamera> camera);

    Scope<EditorWindow> window;

    RenderHandle globalMaterial;
    RenderHandle objectShaderMaterial;
    Scope<GeometryPass> geometryPass;
    Scope<ShadingPass> shadingPass;
    Scope<ShadowPass> shadowPass;
    Scope<EditorCamera> editorCamera;
};

}