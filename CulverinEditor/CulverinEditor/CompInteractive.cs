using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompInteractive : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Activate();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Deactivate();
    }
}
