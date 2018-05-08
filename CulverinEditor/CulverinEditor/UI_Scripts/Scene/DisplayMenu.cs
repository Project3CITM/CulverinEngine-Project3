using CulverinEditor;
using CulverinEditor.EventSystem;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class DisplayMenu : CulverinBehaviour
{
    public GameObject option_menu;
    public GameObject display_button;
    public GameObject audio_button;

    public GameObject screen_button;//display
    public GameObject sound_button;//audio

    public GameObject screen_menu;
    public GameObject screen_return_button;

    public GameObject sound_menu;
    public GameObject sound_return_button;

    void Start()
    {
        option_menu = GetLinkedObject("option_menu");
        display_button = GetLinkedObject("display_button");
        audio_button = GetLinkedObject("audio_button");

        screen_button = GetLinkedObject("screen_button");
        sound_button = GetLinkedObject("sound_button");

        screen_menu = GetLinkedObject("screen_menu");
        screen_return_button = GetLinkedObject("screen_return_button");

        sound_menu = GetLinkedObject("sound_menu");
        sound_return_button = GetLinkedObject("sound_return_button");

        screen_menu.SetActive(false);
        sound_menu.SetActive(false);
    }

    public void OpenScreenMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        option_menu.SetActive(false);
        screen_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(screen_button);

    }
    public void OpenKeyBoardMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        option_menu.SetActive(false);
        sound_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(sound_button);

    }
    public void CloseScreenMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        screen_menu.SetActive(false);
        option_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(screen_button);

    }
    public void CloseSoundMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        sound_menu.SetActive(false);
        option_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(sound_button);

    }
}
