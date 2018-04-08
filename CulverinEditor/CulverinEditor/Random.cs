using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Random
    {
        //
        // Resumen:
        //     Returns a random float number between and min [inclusive] and max [exclusive]
        //     (Read Only).
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int Range(int min, int max);

        //
        // Resumen:
        //     Returns a random float number between and min [inclusive] and max [exclusive]
        //     (Read Only).
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float Range(float min, float max);

    }
}
