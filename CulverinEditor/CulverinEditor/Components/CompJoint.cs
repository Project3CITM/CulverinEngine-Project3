using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompJoint : CulverinBehaviour
    {
        public CompJoint()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DeleteJoint();
    }
}

