using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Material : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetBool(string name, bool value);
    }
}

