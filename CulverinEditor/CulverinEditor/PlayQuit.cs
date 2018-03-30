using CulverinEditor;
using CulverinEditor.SceneManagement;

public class PlayQuit : CulverinBehaviour
{

    void Play()
    {
        SceneManager.LoadScene("NewScene");
    }
    void Quit()
    {
        //Quit
    }
}