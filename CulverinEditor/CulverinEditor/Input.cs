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
        public static extern bool GetKeyDown(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyUp(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyRepeat(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButtonDown(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButtonUp(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButtonRepeat(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Vector3 GetMousePosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseXAxis();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseYAxis();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseMoutionX();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseMoutionY();

    }
}
