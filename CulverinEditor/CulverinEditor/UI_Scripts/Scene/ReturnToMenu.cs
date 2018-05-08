using CulverinEditor;
using CulverinEditor.SceneManagement;

using CulverinEditor.Debug;

public class ReturnToMenu : CulverinBehaviour
{


    void Start()
    {
    
        Audio.StopAllSounds();
        Input.SetInputManagerActive("GUI", true);
        GetComponent<CompAudio>().PlayEvent("PlayUIMusic");
    }


    public void Return()
    {
        if (SceneManager.CheckMultiSceneReady())
        {
            SceneManager.RemoveSecondaryScene();
        }
        SceneManager.LoadScene("3D Menu intro");
    }

  
}
