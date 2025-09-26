using System;
using System.Runtime.CompilerServices;

namespace Zafkiel
{
    internal static class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_CoreTrace(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_CoreDebug(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_CoreInfo(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_CoreWarn(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_CoreError(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_CoreCritical(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_Trace(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_Debug(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_Info(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_Warn(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_Error(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_Critical(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong uuid, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasScript(ulong uuid, Type scriptType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static Object Entity_GetScript(ulong uuid, Type scriptType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetPosition(ulong entityID, out Vector3 outPosition);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetPosition(ulong entityID, ref Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetRotation(ulong entityID, out Quaternion outRotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetRotation(ulong entityID, ref Quaternion rotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetScale(ulong entityID, out Vector3 outScale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetScale(ulong entityID, ref Vector3 scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetPosition(ulong entityID, out Vector3 outPosition);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetPosition(ulong entityID, ref Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetRotation(ulong entityID, out Quaternion outRotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetRotation(ulong entityID, ref Quaternion rotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetScale(ulong entityID, out Vector3 outScale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetScale(ulong entityID, ref Vector3 scale);
    }
}