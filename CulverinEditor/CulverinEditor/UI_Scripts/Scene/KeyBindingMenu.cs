using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class KeyBindingMenu : CulverinBehaviour
{
    GameObject menu;
    GameObject keybinding_menu;
    GameObject keyboard_menu;
    GameObject controller_menu;

    void Start()
    {
        menu = GetLinkedObject("menu");
        keybinding_menu = GetLinkedObject("keybinding_menu");
        keyboard_menu = GetLinkedObject("keyboard_menu");
        controller_menu = GetLinkedObject("controller_menu");

        keybinding_menu.SetActive(false);
    }

    public void OpenKeyBindingMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        menu.SetActive(false);
        keybinding_menu.SetActive(true);
    }
    public void OpenKeyBoardMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        keybinding_menu.SetActive(false);
        keyboard_menu.SetActive(true);
    }
    public void OpenControllerMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        keybinding_menu.SetActive(false);
        controller_menu.SetActive(true);
    }
    public void CloseKeyboardControllerMenu()
    {
        if (Input.GetChangeInputActive())
            return;

        GetComponent<CompAudio>().PlayEvent("Push");
        if(keybinding_menu.IsActive())
            keybinding_menu.SetActive(false);
        if (controller_menu.IsActive())
            controller_menu.SetActive(false);
        keybinding_menu.SetActive(true);
    }
    public void CloseKeyBindingMenu()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        keybinding_menu.SetActive(false);
        menu.SetActive(true);
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
