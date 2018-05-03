using CulverinEditor;
using CulverinEditor.EventSystem;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class AudioMenu : CulverinBehaviour
{
    public GameObject audio_sl;
    public GameObject mute_cb;
    public bool mute = false;
    public float audio_multiplier = 100;

    void Start()
    {
        audio_sl = GetLinkedObject("audio_sl");
        mute_cb = GetLinkedObject("mute_cb");
    }

    public void SetVolume()
    {
        float multiplier = audio_sl.GetComponent<CompSlider>().GetFill();
        Audio.ChangeVolume(multiplier * audio_multiplier);
    }

    public void Mute()
    {
        mute = !mute;
        Audio.Mute(mute);
    }
   
}
