using System;
using Zafkiel;

namespace Sandbox
{
    public class Player : Entity
    {
        void OnCreate()
        {
            Log.Info("Player OnCreate!");
            Log.Info($"Has TransformComponent: {HasComponent<Transform>()}");
            Player player = GetScript<Player>();
            bool flag = HasScript<Player>();
        }
        void OnUpdate(float timestep)
        {
            value_1++;
            var position = Position;
            position.x += timestep * 0.1f;
            Position = position;
            var scale = GetComponent<Transform>().Scale;
            scale.y += timestep * 0.5f;
            Scale = scale;
        }
        public int value_1 = 1;
        public float value_2 = 1.5f;
        public float value_3 = 1.5f;
        void OnDestroy()
        {
            Log.Info("Player OnDestroy!");
        }
    }
}
