using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Audio : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StopAllSounds();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void PauseAllSounds();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ResumeAllSounds();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetAudioVariableValue(string var_name, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ChangeState(string group, string new_state);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ChangeVolume(float volume);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetVolume();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Mute(bool m);
    }
}
