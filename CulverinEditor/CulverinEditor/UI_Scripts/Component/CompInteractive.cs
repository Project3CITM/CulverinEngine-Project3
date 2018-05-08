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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetInteractivity(bool enable);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsNormal();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsHighlighted();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsDisabled();
    }
}
