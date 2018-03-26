using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompAudio : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool PlayEvent(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void StopEvent(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAuxiliarySends(string bus, float value);
    }
}
