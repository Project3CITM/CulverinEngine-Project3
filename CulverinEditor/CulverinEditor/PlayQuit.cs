using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class PlayQuit : CulverinBehaviour
{
    public void Play()
    {
        Debug.Log("GOING TO PLAY");
        Input.SetInputManagerActive("GUI", false);
        SceneManager.LoadScene("PlayerScene");
    }

    public void Quit()
    {
        Debug.Log("GOING TO Quit");
        
    }
}