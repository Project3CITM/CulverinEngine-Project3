using CulverinEditor;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    class CompMaterial : CulverinBehaviour
    {

        public CompMaterial()
        {

        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAlbedo(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetNormals(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAmbientOcclusion(string name);
    }
}

