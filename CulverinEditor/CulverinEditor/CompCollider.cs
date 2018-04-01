using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompCollider : CulverinBehaviour
    {
        public CompCollider()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject GetCollidedObject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetContactPoint();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetContactNormal();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void MoveKinematic(Vector3 pos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void CallOnContact();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void CallOnTriggerEnter();
    }
}
