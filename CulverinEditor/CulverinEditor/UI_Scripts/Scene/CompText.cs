using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    
    public class CompText : CompGraphic
    {
        public CompText()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern CompRectTransform GetRectTransform();



        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern CompText SetText(string text);
    }
}