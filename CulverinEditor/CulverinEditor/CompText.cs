using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    
    public class CompText : CompInteractive
    {
        public CompText()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern CompRectTransform GetRectTransform();
    }
}