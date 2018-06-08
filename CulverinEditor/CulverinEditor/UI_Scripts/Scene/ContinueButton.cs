using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ContinueButton : CulverinBehaviour
{

    public bool reset = false;
    private float volume = 0.0f;

    void Start()
    {
        volume = Audio.GetVolume();
        Audio.ChangeVolume(20);
    }

    public void Play()
    {
        if (reset)
            StatsScore.ResetScore();
        Audio.ChangeVolume(volume);
        Input.SetInputManagerActive("GUI", false);
        SceneManager.RemoveNoDestroy();        
    }

}