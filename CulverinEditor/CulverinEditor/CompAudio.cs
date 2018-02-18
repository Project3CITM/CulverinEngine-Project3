using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompAudio : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void PlayEvent(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void StopEvent(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAuxiliarySends(string bus, float value);
    }
}
