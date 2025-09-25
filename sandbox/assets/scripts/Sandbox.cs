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
