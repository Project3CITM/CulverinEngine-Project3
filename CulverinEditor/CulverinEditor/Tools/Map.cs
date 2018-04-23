using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor.Map
{
    public class Map
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetMapString();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetHeightMap();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetWidthMap();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetSeparation();

    }
}
