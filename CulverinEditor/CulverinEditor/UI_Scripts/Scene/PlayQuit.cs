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
        push.PlayEvent("Push");
        SceneManager.LoadScene("HistoryScene");
    }

    public void Quit()
    {
        push.PlayEvent("Push");
        SceneManager.QuitScene();
    }
}