using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompImage : CompGraphic
    {
        //functions like fillamount
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void FillAmount(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern CompRectTransform GetRectTransform();
    }
}
