using System.Runtime.CompilerServices;
using System;

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
        ///     Load the scene by his name and the current scene will not be erased.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadMultiSceneNoDestroy(string main_scene_name, string secondary_scene_name);

        /// <summary>  
        ///     Check if the loaded scene is over.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool CheckSceneReady();

        /// <summary>  
        ///     Check if the loaded scene is over.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool CheckMultiSceneReady();

        /// <summary>  
        ///     Remove the current scene before doing LoadSceneNoDestroy (Dont Destroy On Load)!
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RemoveNoDestroy();

        /// <summary>  
        ///     Exit Game.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RemoveSecondaryScene();

        /// <summary>  
        ///     Exit Game.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ChangeToSecondaryScene();

        /// <summary>  
        ///     Exit Game.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void BlockGUIinput();

        /// <summary>  
        ///     Exit Game.
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void QuitScene();

        /// <summary>  
        ///     Load Walkability Map
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadNewWalkableMap(string map);

        /// <summary>  
        ///     Load Oclusion Map
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadNewOclusionMap(string map);
        

        /// <summary>  
        ///     SaveInfo Player.
        /// </summary>
        //[Obsolete("Doesn't work at the moment!", true)]
        //[MethodImpl(MethodImplOptions.InternalCall)]
        //public static extern void PushSaveInfo(Vector3 info);

        /// <summary>  
        ///     SaveInfo Player..
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void PushSaveInfo(float hp);

        /// <summary>  
        ///     LoadInfo Player..
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float PopLoadInfo();

    }
}
