using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompParticleSystem : CulverinBehaviour
    {
        public CompParticleSystem()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ActivateEmission(bool active);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsEmitterActive();
    }
}
