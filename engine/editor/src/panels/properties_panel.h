#pragma once

#include "function/scene/components.h"
#include "function/scene/scene.h"
#include "editorGUI/editorGUI.h"
#include "panels/panel.h"

namespace Zafkiel
{

using namespace Reflection;

class PropertiesPanel : public Panel
{
  public:
    PropertiesPanel() {}

    void SetCurrentScene(Ref<Scene> currentScene) { scene = currentScene; }

    virtual void Render() override;

  private:
    void DrawComponents(Entity entity);

    void DrawTagComponent(TagComponent &tagComponent);

    template <typename T>
    void DrawCommonComponent(T &component)
    {
        const Class *typeInfo = GetType<T>()->template As<Class>();
        GUITreeNode node((void *)typeInfo, ImGuiTreeNodeFlags_OpenOnArrow, typeInfo->GetName());
        node.Expand([&]() {
            Any instance = component;
            for (const auto &prop : typeInfo->GetProperties())
            {
                Any subInstance = prop->Call(instance);
                switch (prop->GetTypeInfo()->GetCategory())
                {
                case TypeCategory::Class:
                    if (prop->GetTypeInfo() == GetType<vec2>())
                    {
                        EditorGUI().DragVec2(prop->GetName(), subInstance.As<vec2>());
                    }
                    else if (prop->GetTypeInfo() == GetType<vec3>())
                    {
                        EditorGUI().DragVec3(prop->GetName(), subInstance.As<vec3>());
                    }
                    else if (prop->GetTypeInfo() == GetType<vec4>())
                    {
                        EditorGUI().DragVec4(prop->GetName(), subInstance.As<vec4>());
                    }
                    break;
                case TypeCategory::String:
                    EditorGUI().InputText(prop->GetName(), subInstance.As<std::string>());
                    break;

                default: break;
                }
            }
        });
    }
    Ref<Scene> scene;
};
}