using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Transform : Component
    {
        //protected Transform();        
        public Vector3 local_position {
            get
            {
                return GetPosition();
            }
            set
            {
                SetPosition(value);
            }
        }

        public Vector3 position
        {
            get
            {
                return GetGlobalPosition();
            }
            set
            {
                SetGlobalPosition(value);
            }
        }

        public Vector3 local_rotation
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

        public Vector3 rotation
        {
            get
            {
                return GetGlobalRotation();
            }
            set
            {
                SetGlobalRotation(value);
            }
        }

        public Vector3 forward
        {
            get
            {
                return GetForwardVector();
            }
            set
            {
                SetForwardVector(value);
            }
        }

        public Vector3 backward
        {
            get
            {
                return GetBackwardVector();
            }
            set
            {
                SetBackwardVector(value);
            }
        }

        public Vector3 up
        {
            get
            {
                return GetUpVector();
            }
            set
            {
                SetUpVector(value);
            }
        }

        public Vector3 down
        {
            get
            {
                return GetDownVector();
            }
            set
            {
                SetDownVector(value);
            }
        }

        public Vector3 right
        {
            get
            {
                return GetRightVector();
            }
            set
            {
                SetRightVector(value);
            }
        }

        public Vector3 left
        {
            get
            {
                return GetLeftVector();
            }
            set
            {
                SetLeftVector(value);
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
        public extern Vector3 GetForwardVector();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 SetForwardVector(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetBackwardVector();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 SetBackwardVector(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetUpVector();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 SetUpVector(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetDownVector();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 SetDownVector(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetRightVector();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 SetRightVector(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetLeftVector();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 SetLeftVector(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetGlobalPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetPosition(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetGlobalPosition(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Translate(Vector3 translation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetGlobalRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRotation(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetGlobalRotation(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void RotateAroundAxis(Vector3 value,float angle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetScale();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetScale(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void LookAt(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void LookAtTrans(Transform value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float4x4 GetLocalTransform();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float4x4 GetGlobalTransform();
    }
}
