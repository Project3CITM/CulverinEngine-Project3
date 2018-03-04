using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompCollider : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject GetCollidedObject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void MoveKinematic(Vector3 pos);
    }
}
