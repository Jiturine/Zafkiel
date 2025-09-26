namespace Zafkiel
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class Transform : Component
    {
        public Vector3 Position
        {
            get
            {
                InternalCalls.Transform_GetPosition(Entity.UUID, out Vector3 position);
                return position;
            }
            set
            {
                InternalCalls.Transform_SetPosition(Entity.UUID, ref value);
            }
        }
        public Quaternion Rotation
        {
            get
            {
                InternalCalls.Transform_GetRotation(Entity.UUID, out Quaternion rotation);
                return rotation;
            }
            set
            {
                InternalCalls.Transform_SetRotation(Entity.UUID, ref value);
            }
        }
        public Vector3 Scale
        {
            get
            {
                InternalCalls.Transform_GetScale(Entity.UUID, out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.Transform_SetScale(Entity.UUID, ref value);
            }
        }
    }
}