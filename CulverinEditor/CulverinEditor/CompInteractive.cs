using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompInteractive : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Activate();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Deactivate();

    }
}
