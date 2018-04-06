using System.Runtime.CompilerServices;

namespace CulverinEditor.SceneManagement
{
    public class SceneManager
    {
        //
        // Resumen:
        //     ...
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadScene(string scene_name);

        //
        // Resumen:
        //     ...
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadSceneNoDestroy(string scene_name);

        //
        // Resumen:
        //     ...
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool CheckSceneReady();

        //
        // Resumen:
        //     ...
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RemoveNoDestroy();

        //
        // Resumen:
        //     ...
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void QuitScene();

    }
}
