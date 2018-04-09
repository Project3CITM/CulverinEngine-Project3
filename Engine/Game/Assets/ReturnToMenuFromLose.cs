using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ReturnToMenuFromLose : CulverinBehaviour
{

    void Start()
    {
        Audio.StopAllSounds();
        Input.SetInputManagerActive("GUI", true);
        GetComponent<CompAudio>().PlayEvent("LoseMusic");
    }
    public void Return()
    {
        SceneManager.LoadScene("MainMenuScene");
    }

}
