using System.Runtime.CompilerServices;

namespace CulverinEditor.SceneManagement
{
    public class SceneManager
    {
        /// <summary>  
        ///     Load the scene by his name
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadScene(string scene_name);

        /// <summary>  
        ///     Load the scene by his name and the current scene will not be erased.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadSceneNoDestroy(string scene_name);

        /// <summary>  
        ///     Check if the loaded scene is over.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool CheckSceneReady();

        /// <summary>  
        ///     Remove the current scene before doing LoadSceneNoDestroy (Dont Destroy On Load)!
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RemoveNoDestroy();

        /// <summary>  
        ///     Exit Game.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void QuitScene();

    }
}
