using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ContinueButton : CulverinBehaviour
{
    public void Play()
    {
        Debug.Log("GOING TO GAME");
        Input.SetInputManagerActive("GUI", false);
        SceneManager.LoadScene("FinalMap1");
    }

}