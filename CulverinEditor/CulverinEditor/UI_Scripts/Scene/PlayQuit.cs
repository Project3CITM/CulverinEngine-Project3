using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class PlayQuit : CulverinBehaviour
{
    public void Play()
    {
        Debug.Log("GOING TO PLAY");
        SceneManager.LoadScene("HistoryScene");
    }

    public void Quit()
    {
        Debug.Log("GOING TO Quit");
        SceneManager.QuitScene();
    }
}