#include "properties_panel.h"
#include "function/scene/components.h"
#include "function/engine.h"
#include "function/script/script_engine.h"
#include "script/editor_script_engine.h"
#include "editor.h"

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

    Ref<Scene> scene = Engine::GetActiveScene();
    if (!Editor::GetSelectedEntity()) return;
    Entity entity = Editor::GetSelectedEntity();
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
                        Log::CoreWarn("Entity already has {}!", componentName);
                }
            });
            if (!validType)
            {
                auto scriptEngine = Editor::GetScriptEngine();
                auto &scriptName = componentName;
                if (scriptEngine->HasScript(scriptName))
                {
                    validType = true;
                    if (!entity.HasComponent<ScriptComponent>())
                        entity.AddComponent(ScriptComponent{entity.GetUUID()});
                    auto &scripts = entity.GetComponent<ScriptComponent>().scripts;
                    if (auto it = std::find(scripts.begin(), scripts.end(), scriptName); it != scripts.end())
                    {
                        Log::CoreWarn("Entity already has {}!", scriptName);
                    }
                    else
                    {
                        scripts.push_back(scriptName);
                        scriptEngine->AddEntityScriptInstance(entity.GetUUID(), scriptName);
                        Log::CoreInfo("Instantiate script instance: {}", scriptName);
                    }
                }
            }
            if (!validType) Log::CoreWarn("Unknown Component Type: {}", componentName);
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
            Editor::GetSelectedEntity().RemoveComponent<TransformComponent>();
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

void PropertiesPanel::DrawScriptComponent(ScriptComponent &scriptComponent)
{
    std::vector<std::string>::iterator toBeRemove = scriptComponent.scripts.end();
    for (auto scriptName : scriptComponent.scripts)
    {
        auto scriptInstance = Engine::GetScriptEngine()->GetEntityScriptInstance(Editor::GetSelectedEntity().GetUUID(), scriptName);
        if (scriptInstance)
        {
            GUITreeNode node(scriptName.data(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen, scriptName);
            node.Popup([&]() {
                EditorGUI().MenuItem("Remove", [&]() {
                    Engine::GetScriptEngine()->RemoveEntityScriptInstance(Editor::GetSelectedEntity().GetUUID(), scriptName);
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
        DrawScriptComponent(entity.GetComponent<ScriptComponent>());
    }
}
}