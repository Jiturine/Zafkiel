using System;

namespace Zafkiel
{
    public class Entity
    {
        public Entity() { }
        public Entity(uint id)
        {
            ID = id;
        }
        public readonly uint ID;
        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }
        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }
    }
}