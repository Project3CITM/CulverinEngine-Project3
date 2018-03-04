using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompRigidBody : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetColliderPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Quaternion GetColliderQuaternion();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void RemoveJoint();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void MoveKinematic(Vector3 pos, Quaternion rotation);
    }
}
