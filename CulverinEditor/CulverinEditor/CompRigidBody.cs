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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ApplyForce(Vector3 force);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ApplyImpulse(Vector3 impulse);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ApplyTorqueForce(Vector3 force);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ApplyTorqueImpulse(Vector3 impulse);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void LockTransform();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void UnLockTransform();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ResetForce();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void WakeUp();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAtMaxJointPose();
    }
}
