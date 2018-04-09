using CulverinEditor;
using CulverinEditor.EventSystem;
using CulverinEditor.Debug;
public class InputManager : CulverinBehaviour
{
    public bool gui_active = false;
    public bool gui_block = false;
    public bool player_active = true;
    public bool player_block = false;
    public GameObject selected_interactive;

    // Use this for initialization
    void Start()
    {
        Input.SetInputManagerActive("GUI", gui_active);
        Input.SetInputManagerBlock("GUI", gui_block);
        Input.SetInputManagerActive("Player", player_active);
        Input.SetInputManagerBlock("Player", player_block);
        selected_interactive = GetLinkedObject("selected_interactive");
        if(selected_interactive!=null)
        {
            EventSystem.SendInteractiveSelected(selected_interactive);
        }
    }

}
