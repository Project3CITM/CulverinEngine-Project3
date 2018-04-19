using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public sealed class Time
    {
        public Time() { }

        /// <summary>  
        ///     Returns a random float number between and min [inclusive] and max [exclusive]
        ///     (Read Only).
        /// </summary>  
        public static float deltaTime
        {
            get
            {
                return DeltaTime();
            }
        }

        /// <summary>  
        ///     The real time in seconds since the game started (Read Only).
        /// </summary>  
        public static float realtimeSinceStartup
        {
            get
            {
                return RealtimeSinceStartup();
            }
        }
        /// <summary>  
        ///     The scale at which the time is passing. This can be used for slow motion effects.
        /// </summary>  
        public static float timeScale
        {
            get
            {
                return TimeScale();
            }
            set
            {
                TimeScale(value);
            }
        }

        /// <summary>  
        ///     The time in seconds since you set Time.timePlay = 0; Only use Time.timePlay = 0 one time!
        /// </summary>  
        public static float timePlay
        {
            get
            {
                return TimePlay();
            }
            set
            {
                StartTimePlay(value);
            }
        }

        [Obsolete("Now use variable deltaTime. Example: Time.deltaTime;", false)]
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float DeltaTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float RealtimeSinceStartup();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float TimeScale();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void TimeScale(float time);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StartTimePlay(float time = 0.0f);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float TimePlay();


    }
}
