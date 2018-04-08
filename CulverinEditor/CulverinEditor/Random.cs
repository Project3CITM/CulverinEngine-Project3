using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    /// <summary>  
    /// Class for generating random data.
    /// </summary>  
    public sealed class Random
    {
        /// <summary>  
        ///     Returns a random float number between and min [inclusive] and max [exclusive]
        ///     (Read Only).
        /// </summary>  
        /// <remarks>Updates the record <paramref name="min"/>.
        /// <para>
        /// Use <see cref="DoesRecordExist"/> to verify that ''' the record exists before calling this method.
        /// </para>
        /// </remarks>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int Range(int min, int max);

        /// <summary>  
        ///     Returns a random float number between and min [inclusive] and max [exclusive]
        ///     (Read Only).
        /// </summary>  
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float Range(float min, float max);

    }
}
