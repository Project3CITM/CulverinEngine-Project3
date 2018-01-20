using System.Runtime.CompilerServices;

namespace CulverinEditor.Debug
{
    public class Debug
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Log(string message);
    }
}