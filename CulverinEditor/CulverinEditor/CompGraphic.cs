using System.Runtime.CompilerServices;

namespace CulverinEditor
{

    public class CompGraphic : Component
    {
        //basic graphic funcions like edit raycast target, color, enable, disable on render
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRaycastTarget(bool flag);


    }
}