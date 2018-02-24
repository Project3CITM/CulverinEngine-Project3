using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompAnimation : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void PlayAnimation(string name, bool blending = true);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetTransition(string name, bool condition = true);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsAnimationStopped(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsAnimationRunning(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsAnimOverXTime(float num_between_0_1);
    }
}
