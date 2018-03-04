using System.Runtime.CompilerServices;

namespace CulverinEditor
{

    public class CompGraphic : CulverinBehaviour
    {
        //basic graphic funcions like edit raycast target, color, enable, disable on render
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRaycastTarget(bool flag);




    }
}