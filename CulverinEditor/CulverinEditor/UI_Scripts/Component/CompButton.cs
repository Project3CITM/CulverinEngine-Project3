using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    
    public class CompButton : CompInteractive
    {
        public CompButton()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Clicked();
    }
}