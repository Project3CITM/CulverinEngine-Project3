using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ContinueButton : CulverinBehaviour
{
    public void Play()
    {
        StatsScore.ResetScore();
        Debug.Log("GOING TO GAME");
        SceneManager.LoadScene("FinalMap1");
    }

}