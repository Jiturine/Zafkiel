#include "world.h"
#include "components.h"
#include "core/meta/serializer/custom_serialize.h"

namespace Zafkiel
{

template <typename... T, typename Func>
void for_each_type(std::tuple<T...>, Func &&f)
{
    (f.template operator()<T>(), ...);
}

void Serialization<World>::Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
{
    out << YAML::BeginMap;
    const World &world = instance.As<World>();
    for (auto entity : world.AllEntities())
    {
        UUID uuid = entity.GetUUID();
        out << YAML::Key;
        SerializeAny(uuid, GetType<UUID>(), out);
        out << YAML::Value << YAML::BeginMap;

        for_each_type(ComponentList{}, [&entity, &out]<typename T>() {
            if (entity.HasComponent<T>())
            {
                out << YAML::Key << GetType<T>()->GetName() << YAML::Value;
                const T &component = entity.GetComponent<T>();
                SerializeAny(Any(component), GetType<T>(), out);
            }
        });

        out << YAML::EndMap;
    }
    out << YAML::EndMap;
}

void Serialization<World>::Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data)
{
    World &world = instance.As<World>();
    for (const auto &kvp : data)
    {
        world.SpawnEntityWithUUID(kvp.first.as<uint64_t>());
    }

    for (const auto &kvp : data)
    {
        Entity entity = world.GetEntityByUUID(kvp.first.as<uint64_t>());
        auto &componentData = kvp.second;
        for_each_type(ComponentList{}, [&entity, &componentData]<typename T>() {
            const Type *componentType = GetType<T>();

            if (componentData[componentType->GetName()])
            {
                T component;
                Any componentInstance = component;
                DeserializeAny(componentInstance, componentType, componentData[componentType->GetName()]);
                entity.AddComponent(std::move(component));
            }
        });
    }
}

}