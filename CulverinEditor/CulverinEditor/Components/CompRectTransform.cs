using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompRectTransform : Transform
    {
        public CompRectTransform()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetUIPosition(Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetUIPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 SetWidth(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 SetHeight(int value);
    }

}
