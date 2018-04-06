using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompInteractive : CulverinBehaviour
    {
        public CompInteractive()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Activate();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Deactivate();
    }
}
