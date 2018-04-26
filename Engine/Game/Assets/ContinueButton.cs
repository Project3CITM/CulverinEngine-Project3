using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ContinueButton : CulverinBehaviour
{
    public void Play()
    {       
        StatsScore.ResetScore();
        Debug.Log("Remove asdasdasdadadadsasdasdadasdads",Department.PHYSICS,Color.RED);
        Input.SetInputManagerActive("GUI", false);
        SceneManager.RemoveNoDestroy();        
    }

}