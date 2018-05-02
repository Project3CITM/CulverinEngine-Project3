using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompCheckBox : CompInteractive
    {
        public CompCheckBox()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void HideTick();
    }
}

