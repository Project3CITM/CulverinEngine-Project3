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
    public GameObject audio_button;

    public bool mute = false;
    public float audio_multiplier = 100;


    void Start()
    {
        audio_menu = GetLinkedObject("audio_menu");
        options_menu = GetLinkedObject("options_menu");
        audio_button = GetLinkedObject("audio_button");

        audio_sl = GetLinkedObject("audio_sl");
        mute_cb = GetLinkedObject("mute_cb");

        audio_menu.SetActive(false);

    }

    public void OpenAudioMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        audio_menu.SetActive(true);
        options_menu.SetActive(false);
        EventSystem.SendInteractiveSelected(audio_sl);
    }
    public void CloseScreenOptions()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        audio_menu.SetActive(false);
        options_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(audio_button);
    }

    public void Mute()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        mute = !mute;
        Audio.Mute(mute);
    }
   
}
