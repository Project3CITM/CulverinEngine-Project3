using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Material : CulverinBehaviour
    {
        public Material()
        { }
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetBool(string name, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetFloat(string name, float value);

    }
}

