using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompLight : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetIntensity();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetIntensity(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetConstant();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetConstant(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetLinear();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetLinear(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetQuadratic();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetQuadratic(float value);
    }
}

