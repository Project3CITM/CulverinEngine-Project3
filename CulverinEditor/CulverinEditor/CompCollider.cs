using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompCollider : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject GetCollidedObject();
    }
}
