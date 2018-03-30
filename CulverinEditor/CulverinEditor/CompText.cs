using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    
    public class CompText : CompInteractive
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern CompRectTransform GetRectTransform();
    }
}