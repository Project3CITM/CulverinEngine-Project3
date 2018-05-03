using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class InputShow : CulverinBehaviour
{
    string last_text;
    string text;
    string action_name = null;
    string input_name = null;
    int device = 0;
    bool update = false;

    void Start()
    {
        update = true;
        device = Input.GetActualDeviceCombo();


        action_name = "Interact";
        input_name = "Player";
        text = "Pulse ";
        text += Input.GetInput_ControllerKeyBindingName(action_name, input_name, device, false);
        text += " to Interact";
        last_text = text;
        GetComponent<CompText>().SetText(text);

    }
    void Update()
    {
        if (!update)
            Start();
        device = Input.GetActualDeviceCombo();
        text = "Pulse ";
        text += Input.GetInput_ControllerKeyBindingName(action_name, input_name, device, negative_key);
        text += " to Interact";

        if (last_text!=text)
        {
            GetComponent<CompText>().SetText(text);
            last_text = text;
        }
    }

}