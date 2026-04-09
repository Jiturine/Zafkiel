#include "Function/Scene/Scene.h"
#include "Function/Scene/Components.h"
#include "Function/Script/ScriptEngine.h"

template <typename... T, typename Func>
void for_each_type(std::tuple<T...>, Func &&f)
{
    (f.template operator()<T>(), ...);
}

namespace Zafkiel
{
std::string Scene::Serialize() const
{
    YamlSerializer out;
    out.BeginMap();
    for (auto entity : world.AllEntities())
    {
        UUID uuid = entity.GetUUID();
        out.Key(uuid).BeginMap();

        for_each_type(ComponentList{}, [&entity, &out]<typename T>() {
            if (entity.HasComponent<T>())
            {
                out.Key(GetType<T>()->GetName());
                const T &component = entity.GetComponent<T>();
                if (GetType<T>() == GetType<ScriptComponent>())
                    out.Value(component, ScriptEngine::GetScriptInstances(entity.GetUUID()));
                else
                    out.Value(component);
            }
        });

        out.EndMap();
    }
    out.EndMap();
    return out.c_str();
}

// std::string Scene::SerializeScripts() const
// {
//     YamlSerializer out;
//     out.BeginMap();
//     for (auto entity : world.AllEntities())
//     {
//         if (entity.HasComponent<ScriptComponent>())
//         {
//             UUID uuid = entity.GetUUID();
//             out.Key(uuid).Value(component, Engine::GetScriptEngine()->GetScriptInstances(entity.GetUUID()));
//         }

//         for_each_type(ComponentList{}, [&entity, &out]<typename T>() {
//             if (entity.HasComponent<T>())
//             {
//                 out.Key(GetType<T>()->GetName());
//                 const T &component = entity.GetComponent<T>();
//                 if (GetType<T>() == GetType<ScriptComponent>())
//                     out.Value();
//                 else
//                     out.Value(component);
//             }
//         });

//         out.EndMap();
//     }
//     out.EndMap();
//     return out.c_str();
// }

void Scene::Deserialize(const std::string &str)
{
    YamlDeserializer data(str);
    for (auto &item : data.MapItems())
    {
        UUID uuid = item.first.As<UUID>();
        world.SpawnEntityWithUUID(uuid);
    }

    for (auto &item : data.MapItems())
    {
        UUID uuid = item.first.As<UUID>();
        Entity entity = world.GetEntityByUUID(uuid);
        auto &componentData = data[uuid];
        for_each_type(ComponentList{}, [&]<typename T>() {
            const Type *componentType = GetType<T>();

            if (componentData[componentType->GetName()])
            {
                T component;
                if (componentType == GetType<ScriptComponent>())
                {
                    auto context = entity;
                    component = componentData[componentType->GetName()].As<T>(context);
                }
                else
                {
                    component = componentData[componentType->GetName()].As<T>(world);
                }
                entity.AddComponent(MoveTemp(component));
            }
        });
    }
}

}