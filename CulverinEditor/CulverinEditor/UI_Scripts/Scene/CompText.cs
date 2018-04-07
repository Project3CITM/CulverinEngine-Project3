using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    
    public class CompText : CompInteractive
    {
        public CompText()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern CompRectTransform GetRectTransform();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern CompText SetAlpha(float alpha);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern CompText SetText(string text);
    }
}