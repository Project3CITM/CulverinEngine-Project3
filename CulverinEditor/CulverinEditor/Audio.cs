using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Audio
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
       public static extern void StopSounds();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void PauseSounds();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ResumeSounds();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetAudioVariableValue(string variable_name, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void externChangeState(string group, string new_state);
    }
}
