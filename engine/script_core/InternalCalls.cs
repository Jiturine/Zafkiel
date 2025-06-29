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
        internal extern static bool Entity_HasComponent(uint ID, Type componentType);
    }
}