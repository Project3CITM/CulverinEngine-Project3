using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.EventSystem;
using CulverinEditor.Debug;

public class PlayExit : CulverinBehaviour
{
    public GameObject pop_up_menu;
    public GameObject pop_up_menu_button;
    public GameObject quit_button;
    public GameObject play_button;

    void Start()
    {
        pop_up_menu = GetLinkedObject("pop_up_menu");
        pop_up_menu_button = GetLinkedObject("pop_up_menu_button");
        quit_button = GetLinkedObject("quit_button");
        play_button = GetLinkedObject("play_button");

        pop_up_menu.SetActive(false);
    }

    public void PlayButton()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        SceneManager.LoadScene("HistoryScene");
    }

    public void QuitButton()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        play_button.GetComponent<CompButton>().SetInteractivity(false);
        quit_button.GetComponent<CompButton>().SetInteractivity(false);

        pop_up_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(pop_up_menu_button);
    }
    public void YesButton()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        SceneManager.QuitScene();
    }
    public void NoButton()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        play_button.GetComponent<CompButton>().SetInteractivity(true);
        quit_button.GetComponent<CompButton>().SetInteractivity(true);
        pop_up_menu.SetActive(false);
        EventSystem.SendInteractiveSelected(quit_button);

    }
}