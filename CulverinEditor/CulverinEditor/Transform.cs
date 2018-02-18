using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Transform : Component
    {
        //protected Transform();        
        public Vector3 position {
            get
            {
                return GetPosition();
            }
            set
            {
                SetPosition(value);
            }
        }

        public Vector3 rotation
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

        public Vector3 scale
        {
            get
            {
                return GetScale();
            }
            set
            {
                SetScale(value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetPosition(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Translate(Vector3 translation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRotation(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void RotateAroundAxis(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetScale();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetScale(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void LookAt(Vector3 value);
    }
}
