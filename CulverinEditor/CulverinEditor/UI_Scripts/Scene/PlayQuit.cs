using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class PlayQuit : CulverinBehaviour
{

    void Start()
    {
    }

    public void Play()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        SceneManager.LoadScene("HistoryScene");
    }

    public void Quit()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        SceneManager.QuitScene();
    }

    public void RetryLevel()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        SceneManager.LoadNewOclusionMap(SceneManager.GetNameActualOclusionMap());
        SceneManager.LoadScene(SceneManager.GetNameActualScene());
    }
}