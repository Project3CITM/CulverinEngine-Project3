using CulverinEditor;
using CulverinEditor.EventSystem;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class AudioMenu : CulverinBehaviour
{
    public GameObject audio_menu;
    public GameObject audio_sl;
    public GameObject mute_cb;

    public GameObject options_menu;
    public GameObject input_button;

    public bool mute = false;
    public float audio_multiplier = 100;


    void Start()
    {
        audio_sl = GetLinkedObject("audio_sl");
        mute_cb = GetLinkedObject("mute_cb");
    }

    public void CloseScreenOptions()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        audio_menu.SetActive(false);
        options_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(input_button);
    }

    public void Mute()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        mute = !mute;
        Audio.Mute(mute);
    }
   
}
