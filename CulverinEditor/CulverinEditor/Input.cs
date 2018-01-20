using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Input
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool KeyDown(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool KeyUp(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool KeyRepeat(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool MouseButtonDown(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool MouseButtonUp(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool MouseButtonRepeat(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Vector3 GetMousePosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseXAxis();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseYAxis();
    }
}
