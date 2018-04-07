using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class PlayQuit : CulverinBehaviour
{
    CompAudio push;

    void Start()
    {
        push = GetComponent<CompAudio>();
    }

    public void Play()
    {
        Debug.Log("GOING TO PLAY");
        push.PlayEvent("Push");
        SceneManager.LoadScene("HistoryScene");
    }

    public void Quit()
    {
        Debug.Log("GOING TO Quit");
        SceneManager.QuitScene();
    }
}