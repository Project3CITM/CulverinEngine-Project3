using CulverinEditor;
using CulverinEditor.EventSystem;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class Volume : CulverinBehaviour
{
    public float audio_multiplier = 100;
    float value = 0.0f;
    float last_value = 0.0f;

    bool update = false;

    void Start()
    {
        update = true;
        value= Audio.GetVolume()/ audio_multiplier;
        last_value = value;
        GetComponent<CompSlider>().SetFill(value);
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
        value = GetComponent<CompSlider>().GetFill();
        if (value == last_value)
            return;
        if (Audio.IsMuted()==false)
        {
            Audio.ChangeVolume(value * audio_multiplier);
        }
        last_value = value;
    }

}
