using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ContinueButton : CulverinBehaviour
{
    public void Play()
    {       
        StatsScore.ResetScore();
        Input.SetInputManagerActive("GUI", false);
        SceneManager.RemoveNoDestroy();        
    }

}