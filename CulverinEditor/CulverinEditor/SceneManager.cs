using System.Runtime.CompilerServices;

namespace CulverinEditor.SceneManagement
{
    public class SceneManager
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadScene(object scene_name);
    }
}
