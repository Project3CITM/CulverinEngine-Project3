using System.Runtime.CompilerServices;

namespace CulverinEditor
{

    public class CompGraphic : CulverinBehaviour
    {
        public CompGraphic()
        { }

        //basic graphic funcions like edit raycast target, color, enable, disable on render
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRaycastTarget(bool flag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void  ActivateRender();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DeactivateRender();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetColor(Vector3 color, float alpha);
    }
}