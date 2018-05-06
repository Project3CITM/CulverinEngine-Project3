using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ContinueButton : CulverinBehaviour
{

    public bool reset = false;

    public void Play()
    {
        if (reset)
            StatsScore.ResetScore();
        Input.SetInputManagerActive("GUI", false);
        SceneManager.RemoveNoDestroy();        
    }

}