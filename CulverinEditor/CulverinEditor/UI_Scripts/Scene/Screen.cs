using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor.Screen
{
    public class Screen
    {
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool SetFullScreen();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetFullScreen();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool SetWindowed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetWindowed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool SetBordeless();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetBordeless();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool SetVSync();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool ShowFPS();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool SwapControllerVibration();
    }
}
