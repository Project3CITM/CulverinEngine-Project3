using System.Runtime.CompilerServices;

namespace CulverinEditor.SceneManagement
{
    public class SceneManager
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadScene(object scene_name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadSceneNoDestroy(object scene_name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool CheckSceneReady(object scene_name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ChangeToScene(object scene_name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void QuitScene();

    }
}
