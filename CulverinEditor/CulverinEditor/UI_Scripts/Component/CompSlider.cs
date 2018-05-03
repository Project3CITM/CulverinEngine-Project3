using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompSlider : CompInteractive
    {
        public CompSlider()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetFill();
    }
}

