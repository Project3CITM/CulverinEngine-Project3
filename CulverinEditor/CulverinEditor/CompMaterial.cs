using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompMaterial : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAlbedo(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetNormals(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAmbientOcclusion(string name);
    }
}

