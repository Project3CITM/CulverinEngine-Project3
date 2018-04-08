using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ReturnToMenuFromLose : CulverinBehaviour
{

    void Start()
    {
        Audio.StopAllSounds();
        Input.SetInputManagerActive("GUI", true);
        GetComponent<CompAudio>().PlayEvent("PlayLoseMusic");
    }
    public void Return()
    {
        Audio.StopAllSounds();
        GetComponent<CompAudio>().PlayEvent("PlayUIMusic");
        SceneManager.LoadScene("MainMenuScene");
    }

}
