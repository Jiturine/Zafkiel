#include "editor/panels/properties_panel.h"
#include "function/scene/components.h"
#include "function/script/script_engine.h"
#include "editor/function/script/editor_script_engine.h"
#include "editor/editor_context/editor_context.h"
#include "function/scene/scene_manager.h"

namespace Zafkiel
{
template <typename... T, typename Func>
void for_each_type(std::tuple<T...>, Func &&f)
{
    (f.template operator()<T>(), ...);
}

void PropertiesPanel::Render()
{
    GUIWindow propertiesPanel("Properties");

    auto scene = SceneManager::Instance().GetActiveScene();
    auto &selectionContext = EditorContext::GetSelectionContext();
    if (selectionContext.type != SelectionContext::Type::Entity) return;
    Entity entity = selectionContext.entity;
    if (entity)
    {
        static std::string componentName;
        EditorGUI().Button("+", [&]() {
            bool validType = false;
            for_each_type(ComponentList{}, [&]<typename T>() {
                if (Reflection::GetType<T>()->GetName() == componentName)
                {
                    validType = true;
                    if (!entity.HasComponent<T>())
                        entity.AddComponent(T{});
                    else
                        Log::Warn("Entity already has {}!", componentName);
                }
            });
            if (!validType)
            {
                auto &scriptName = componentName;
                if (ScriptEngine::HasScript(scriptName))
                {
                    validType = true;
                    if (!entity.HasComponent<ScriptComponent>())
                        entity.AddComponent(ScriptComponent{entity.GetUUID()});
                    auto &scripts = entity.GetComponent<ScriptComponent>().scripts;
                    if (auto it = std::find(scripts.begin(), scripts.end(), scriptName); it != scripts.end())
                    {
                        Log::Warn("Entity already has {}!", scriptName);
                    }
                    else
                    {
                        scripts.push_back(scriptName);
                        ScriptEngine::AddScriptInstance(entity.GetUUID(), scriptName);
                        Log::Info("Instantiate script instance: {}", scriptName);
                    }
                }
            }
            if (!validType) Log::Warn("Unknown Component Type: {}", componentName);
            componentName.clear();
        });
        EditorGUI().InputText("##Add Component", componentName);

        ImGui::Separator();

        DrawComponents(entity);
    }
}

void PropertiesPanel::DrawTagComponent(TagComponent &tagComponent)
{
    EditorGUI().InputText("Name", tagComponent.name);
}

void PropertiesPanel::DrawTransformComponent(TransformComponent &transformComponent)
{
    const Type *typeInfo = GetType<TransformComponent>();
    GUITreeNode node((void *)typeInfo, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen, typeInfo->GetName());
    node.Popup([&]() {
        EditorGUI().MenuItem("Remove", [&]() {
            // Editor::GetSelectedEntity().RemoveComponent<TransformComponent>();
        });
    });
    node.Expand([&]() {
        vec3 position = transformComponent.position;
        EditorGUI().DragVec3("Position", position, [&]() {
            transformComponent.SetPosition(position);
        });

        vec3 eulerAngles = Maths::EulerDegrees(transformComponent.rotation);
        EditorGUI().DragVec3("Rotation", eulerAngles, [&]() {
            transformComponent.SetRotation(Maths::EulerDrgreesToQuaternion(eulerAngles));
        });

        vec3 scale = transformComponent.scale;
        EditorGUI().DragVec3("Scale", scale, [&]() {
            transformComponent.SetScale(scale);
        });
    });
}

void PropertiesPanel::DrawLightComponent(LightComponent &lightComponent)
{
    const Type *typeInfo = GetType<LightComponent>();
    GUITreeNode node((void *)typeInfo, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen, typeInfo->GetName());
    node.Popup([&]() {
        EditorGUI().MenuItem("Remove", [&]() {
            // Editor::GetSelectedEntity().RemoveComponent<LightComponent>();
        });
    });
    node.Expand([&]() {
        static const char *items[] = {"Directional", "Point", "Spot"};
        static int currentItem = lightComponent.type == LightType::Directional ? 0 : lightComponent.type == LightType::Point ? 1
                                                                                                                             : 2;
        if (ImGui::BeginCombo("Light Type", items[currentItem])) // label + 当前显示内容
        {
            for (int i = 0; i < 3; i++)
            {
                bool isSelected = (currentItem == i);
                if (ImGui::Selectable(items[i], isSelected))
                {
                    currentItem = i;
                    if (i == 0) lightComponent.type = LightType::Directional;
                    else if (i == 1) lightComponent.type = LightType::Point;
                    else lightComponent.type = LightType::Spot;
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        EditorGUI().DragVec3("Color", lightComponent.color);
        EditorGUI().DragFloat("Intensity", lightComponent.intensity);
        if (lightComponent.type == LightType::Directional)
        {
            EditorGUI().DragVec3("Direction", lightComponent.direction);
        }
        else if (lightComponent.type == LightType::Point)
        {
            EditorGUI().DragFloat("Radius", lightComponent.radius);
        }
        else
        {
            EditorGUI().DragFloat("Spot Angle", lightComponent.spotAngle);
        }
    });
}

void PropertiesPanel::DrawScriptComponent(Entity entity, ScriptComponent &scriptComponent)
{
    std::vector<std::string>::iterator toBeRemove = scriptComponent.scripts.end();
    for (auto scriptName : scriptComponent.scripts)
    {
        auto scriptInstance = ScriptEngine::GetScriptInstance(entity.GetUUID(), scriptName);
        if (scriptInstance)
        {
            GUITreeNode node(scriptName.data(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen, scriptName);
            node.Popup([&]() {
                EditorGUI().MenuItem("Remove", [&]() {
                    ScriptEngine::RemoveScriptInstance(entity.GetUUID(), scriptName);
                    toBeRemove = std::find(scriptComponent.scripts.begin(), scriptComponent.scripts.end(), scriptName);
                });
            });
            node.Expand([&]() {
                const auto &fields = scriptInstance->GetScriptClass()->GetFields();
                for (const auto &[name, field] : fields)
                {
                    if (field.type == ScriptFieldType::Float)
                    {
                        float data = scriptInstance->GetFieldValue<float>(name);
                        if (ImGui::DragFloat(name.c_str(), &data)) { scriptInstance->SetFieldValue<float>(name, data); }
                    }
                    if (field.type == ScriptFieldType::Int32)
                    {
                        int data = scriptInstance->GetFieldValue<int32_t>(name);
                        if (ImGui::DragInt(name.c_str(), &data)) { scriptInstance->SetFieldValue<int>(name, data); }
                    }
                    if (field.type == ScriptFieldType::Vector2)
                    {
                        vec2 data = scriptInstance->GetFieldValue<vec2>(name);
                        if (ImGui::DragFloat2(name.c_str(), (float *)&data)) { scriptInstance->SetFieldValue<vec2>(name, data); }
                    }
                    if (field.type == ScriptFieldType::Vector3)
                    {
                        vec3 data = scriptInstance->GetFieldValue<vec3>(name);
                        if (ImGui::DragFloat3(name.c_str(), (float *)&data)) { scriptInstance->SetFieldValue<vec3>(name, data); }
                    }
                    if (field.type == ScriptFieldType::Vector4)
                    {
                        vec4 data = scriptInstance->GetFieldValue<vec4>(name);
                        if (ImGui::DragFloat4(name.c_str(), (float *)&data)) { scriptInstance->SetFieldValue<vec4>(name, data); }
                    }
                }
            });
        }
    }
    if (toBeRemove != scriptComponent.scripts.end())
    {
        scriptComponent.scripts.erase(toBeRemove);
    }
}

void PropertiesPanel::DrawComponents(Entity entity)
{
    if (entity.HasComponent<TagComponent>())
    {
        DrawTagComponent(entity.GetComponent<TagComponent>());
    }
    if (entity.HasComponent<TransformComponent>())
    {
        DrawTransformComponent(entity.GetComponent<TransformComponent>());
    }
    if (entity.HasComponent<SpriteRendererComponent>())
    {
        DrawCommonComponent<SpriteRendererComponent>(entity.GetComponent<SpriteRendererComponent>());
    }
    if (entity.HasComponent<ScriptComponent>())
    {
        DrawScriptComponent(entity, entity.GetComponent<ScriptComponent>());
    }
    if (entity.HasComponent<MeshComponent>())
    {
        DrawCommonComponent(entity.GetComponent<MeshComponent>());
    }
    if (entity.HasComponent<LightComponent>())
    {
        DrawLightComponent(entity.GetComponent<LightComponent>());
    }
    if (entity.HasComponent<MaterialComponent>())
    {
        DrawCommonComponent(entity.GetComponent<MaterialComponent>());
    }
}
}