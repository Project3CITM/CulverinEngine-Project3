using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    
    public class CompButton : CompInteractive
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Clicked();
    }
}