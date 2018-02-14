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
        public static extern bool KeyDown(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool KeyUp(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool KeyRepeat(KeyCode key);

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
