namespace Zafkiel
{
    public class Log
    {
        internal static void CoreTrace(string message)
        {
            InternalCalls.Log_CoreTrace(message);
        }
        internal static void CoreDebug(string message)
        {
            InternalCalls.Log_CoreDebug(message);
        }
        internal static void CoreInfo(string message)
        {
            InternalCalls.Log_CoreInfo(message);
        }
        internal static void CoreWarn(string message)
        {
            InternalCalls.Log_CoreWarn(message);
        }
        internal static void CoreError(string message)
        {
            InternalCalls.Log_CoreError(message);
        }
        internal static void CoreCritical(string message)
        {
            InternalCalls.Log_CoreCritical(message);
        }

        public static void Trace(string message)
        {
            InternalCalls.Log_Trace(message);
        }
        public static void Debug(string message)
        {
            InternalCalls.Log_Debug(message);
        }
        public static void Info(string message)
        {
            InternalCalls.Log_Info(message);
        }
        public static void Warn(string message)
        {
            InternalCalls.Log_Warn(message);
        }
        public static void Error(string message)
        {
            InternalCalls.Log_Error(message);
        }
        public static void Critical(string message)
        {
            InternalCalls.Log_Critical(message);
        }
    }
}