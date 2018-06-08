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
    public GameObject main_menu;
    public GameObject options_button;
    public GameObject options_menu;
    public GameObject display_button;
    public GameObject difficulty_menu;
    public GameObject easy_dif_button;


    void Start()
    {
        pop_up_menu = GetLinkedObject("pop_up_menu");
        pop_up_menu_button = GetLinkedObject("pop_up_menu_button");
        quit_button = GetLinkedObject("quit_button");
        play_button = GetLinkedObject("play_button");
        options_button = GetLinkedObject("options_button");
        options_menu = GetLinkedObject("options_menu");
        display_button = GetLinkedObject("display_button");
        main_menu = GetLinkedObject("main_menu");
        difficulty_menu = GetLinkedObject("difficulty_menu");

        options_menu.SetActive(false);
        pop_up_menu.SetActive(false);
        difficulty_menu.SetActive(false);
    }

    public void PlayButton()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        SceneManager.LoadScene("HistoryScene");
    }

    public void OptionButton()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        options_menu.SetActive(true);
        main_menu.SetActive(false);
        EventSystem.SendInteractiveSelected(display_button);
    }

    public void ReturnFromOptions()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        main_menu.SetActive(true);
        options_menu.SetActive(false);
        EventSystem.SendInteractiveSelected(options_button);
    }

    public void QuitButton()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        pop_up_menu.SetActive(true);
        main_menu.SetActive(false);
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
        pop_up_menu.SetActive(false);
        main_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(quit_button);

    }

    public void DifficultyButton()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        difficulty_menu.SetActive(true);
        main_menu.SetActive(false);
        EventSystem.SendInteractiveSelected(easy_dif_button);
    }

    public void ReturnFromDifficulty()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        difficulty_menu.SetActive(false);
        main_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(play_button);
    }
}