using CulverinEditor;
using CulverinEditor.EventSystem;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class Volume : CulverinBehaviour
{
    public GameObject audio_sl;
    public GameObject mute_cb;
    public bool mute = false;
    public float audio_multiplier = 100;
    float value = 0.0f;
    float last_value = 0.0f;

    bool update = false;
    void Start()
    {
        update = true;
        audio_sl = GetLinkedObject("audio_sl");
        mute_cb = GetLinkedObject("mute_cb");
        value = audio_sl.GetComponent<CompSlider>().GetFill();
        Audio.ChangeVolume(value * audio_multiplier);
        last_value = value;

    }

    void Update()
    {
        if(!update)
        {
            Start();
        }
        SetVolume();
    }

    public void SetVolume()
    {
        value = audio_sl.GetComponent<CompSlider>().GetFill();
        if (value == last_value)
            return;
        Audio.ChangeVolume(value * audio_multiplier);
        last_value = value;
    }

}
