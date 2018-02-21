using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompRigidBody : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetGlobalPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Quaternion GetGlobalQuaternion();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void MoveKinematic(Vector3 pos, Quaternion rotation);
    }
}
