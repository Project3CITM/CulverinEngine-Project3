using System;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class PhysX : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static GameObject RayCast(Vector3 origin, Vector3 direction, float distance);
    }
}
