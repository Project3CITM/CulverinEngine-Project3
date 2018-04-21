using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.EventSystem;

using CulverinEditor.Debug;

public class PauseMenuButtons : CulverinBehaviour
{

    public GameObject pop_up_menu;
    public GameObject pop_up_menu_button;
    public GameObject menu_button;
    public GameObject return_button;

    void Start()
    {
        pop_up_menu = GetLinkedObject("pop_up_menu");
        pop_up_menu_button = GetLinkedObject("pop_up_menu_button");
        menu_button = GetLinkedObject("menu_button");
        return_button = GetLinkedObject("return_button");

        pop_up_menu.SetActive(false);
    }
    void Update()
    {
        if (Input.GetInputManagerActive("GUI"))
        {
            if (!EventSystem.GetInteractiveSelectedActive())
            {
                EventSystem.SendInteractiveSelected(menu_button);
            }
        }
    }
    public void Resume()
    {
        if (SceneManager.CheckMultiSceneReady())
        {
            GetComponent<CompAudio>().PlayEvent("Push");

            SceneManager.ChangeToSecondaryScene();
        }
    }

    public void ReturnToMenu()
    {
        menu_button.GetComponent<CompButton>().SetInteractivity(false);
        return_button.GetComponent<CompButton>().SetInteractivity(false);
        GetComponent<CompAudio>().PlayEvent("Push");

        pop_up_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(pop_up_menu_button);
    }
    public void YesButton()
    {
        GetComponent<CompAudio>().PlayEvent("Push");

        SceneManager.LoadScene("MainMenuScene");
    }
    public void NoButton()
    {
        menu_button.GetComponent<CompButton>().SetInteractivity(true);
        return_button.GetComponent<CompButton>().SetInteractivity(true);
        pop_up_menu.SetActive(false);
        GetComponent<CompAudio>().PlayEvent("Push");

        EventSystem.SendInteractiveSelected(return_button);

    }
}
