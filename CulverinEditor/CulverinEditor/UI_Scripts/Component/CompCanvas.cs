using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompCanvas : CulverinBehaviour
    {
        public CompCanvas()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern CompText SetCanvasAlpha(float alpha);
    }
}
