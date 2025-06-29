using System;
using Zafkiel;

namespace Sandbox
{
    public class Player : Entity
    {
        void OnCreate()
        {
            bool res = HasComponent<Transform>();
            Log.Trace($"HasComponent<Transform>: {res}");
        }
        void OnUpdate(float timestep)
        {
        }
    }
}
