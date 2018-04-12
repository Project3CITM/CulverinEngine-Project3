using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompAnimation : CulverinBehaviour
    {

        public CompAnimation()
        {

        }

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetClipsSpeed(float speed_value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetClipDuration(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetClipDuration(string name, float duration);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetFirstActiveBlendingClip(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetFirstActiveBlendingClipWeight(float weight);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetSecondActiveBlendingClip(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetSecondActiveBlendingClipWeight(float weight);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetBlendInTime(string name, float time);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void PlayAnimationNode(string node_name);
    }
}
