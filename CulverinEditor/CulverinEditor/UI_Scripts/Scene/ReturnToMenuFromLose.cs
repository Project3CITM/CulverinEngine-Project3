using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.EventSystem;

using CulverinEditor.Debug;

public class ReturnToMenuFromLose : CulverinBehaviour
{
    GameObject pop_up_menu;
    GameObject pop_up_menu_button;
    GameObject quit_button;
    GameObject play_button;

    void Start()
    {
        pop_up_menu = GetLinkedObject("pop_up_menu");
        pop_up_menu_button = GetLinkedObject("pop_up_menu_button");
        quit_button = GetLinkedObject("quit_button");
        play_button = GetLinkedObject("play_button");
        Audio.StopAllSounds();
        Input.SetInputManagerActive("GUI", true);
        GetComponent<CompAudio>().PlayEvent("PlayLoseMusic");
    }
    public void Return()
    {
        Audio.StopAllSounds();
        GetComponent<CompAudio>().PlayEvent("PlayUIMusic");
        GetComponent<CompAudio>().PlayEvent("Push");

        SceneManager.LoadScene("MainMenuScene");
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
        if (SceneManager.CheckMultiSceneReady())
        {
            SceneManager.RemoveSecondaryScene();
        }
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
