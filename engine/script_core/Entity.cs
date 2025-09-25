using System;

namespace Zafkiel
{
    public class Entity
    {
        public Entity() { }
        public Entity(ulong uuid)
        {
            _uuid = uuid;
        }
        private readonly ulong _uuid;
        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(_uuid, componentType);
        }
        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }

        public bool HasScript<T>() where T : Entity
        {
            Type scriptType = typeof(T);
            return InternalCalls.Entity_HasScript(_uuid, scriptType);
        }

        public T GetScript<T>() where T : Entity
        {
            if(!HasScript<T>())
                return null;

            Type scriptType = typeof(T);
            return (T)InternalCalls.Entity_GetScript(_uuid, scriptType);
        }
    }
}