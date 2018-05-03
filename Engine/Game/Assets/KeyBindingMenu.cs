using CulverinEditor;
using CulverinEditor.EventSystem;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class KeyBindingMenu : CulverinBehaviour
{

    GameObject option_menu;
    GameObject input_button;

    GameObject keybinding_menu;
    GameObject keyboard_button;
    GameObject controller_button;

    GameObject keyboard_menu;
    GameObject keyboard_return_button;

    GameObject controller_menu;
    GameObject controller_return_button;

    void Start()
    {
        option_menu = GetLinkedObject("option_menu");
        input_button = GetLinkedObject("input_button");

        keybinding_menu = GetLinkedObject("keybinding_menu");
        keyboard_button = GetLinkedObject("keyboard_button");
        controller_button = GetLinkedObject("controller_button");

        keyboard_menu = GetLinkedObject("keyboard_menu");
        keyboard_return_button = GetLinkedObject("keyboard_return_button");

        controller_menu = GetLinkedObject("controller_menu");
        controller_menu = GetLinkedObject("controller_return_button");

        keybinding_menu.SetActive(false);
        keyboard_menu.SetActive(false);
        controller_menu.SetActive(false);

    }

    public void OpenKeyBindingMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        option_menu.SetActive(false);
        keybinding_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(keyboard_button);

    }
    public void OpenKeyBoardMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        keybinding_menu.SetActive(false);
        keyboard_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(keyboard_return_button);

    }
    public void OpenControllerMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        keybinding_menu.SetActive(false);
        controller_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(controller_return_button);

    }
    public void CloseKeyboardControllerMenu()
    {
        if (Input.GetChangeInputActive())
            return;

        GetComponent<CompAudio>().PlayEvent("Push");
        if(keyboard_menu.IsActive())
        {
            keybinding_menu.SetActive(false);
            EventSystem.SendInteractiveSelected(keyboard_button);
        }
        else if (controller_menu.IsActive())
        {
            controller_menu.SetActive(false);
            EventSystem.SendInteractiveSelected(controller_button);

        }
        keybinding_menu.SetActive(true);
    }
    public void CloseKeyBindingMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        keybinding_menu.SetActive(false);
        option_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(input_button);

    }
    public void SavePlayerAction()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        Input.SavePlayerAction();
    }
    public void LoadDefaultPlayerAction()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        Input.LoadDefaultPlayerAction();
    }
}
