using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Transform : Component
    {
        //protected Transform();        
        public Vector3 Position {
            get
            {
                return GetPosition();
            }
            set
            {
                SetPosition(value);
            }
        }

        public Vector3 Rotation
        {
            get
            {
                return GetRotation();
            }
            set
            {
                SetRotation(value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Vector3 GetPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetPosition(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Vector3 GetRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetRotation(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void RotateAroundAxis(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Vector3 GetScale();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetScale(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void LookAt(Vector3 value);
    }
}
