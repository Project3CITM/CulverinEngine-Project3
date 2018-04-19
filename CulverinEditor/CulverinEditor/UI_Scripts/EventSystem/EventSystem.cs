using System.Runtime.CompilerServices;

namespace CulverinEditor.EventSystem
{
    public class EventSystem 
    {


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SendInteractiveSelected(GameObject send);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetInteractiveSelectedActive();
    }
}
