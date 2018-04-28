using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ShowInputName : CulverinBehaviour
{
    string last_text;

    string text;
    string action_name;
    string input_name;
    string device;
    bool negative_key;

    void Start()
    {
        text = Input.GetInput_ControllerKeyBindingName(action_name, input_name, device, negative_key);
        last_text = text;
        GetComponent<CompText>().SetText(text);

    }
    void Update()
    {
        text = Input.GetInput_ControllerKeyBindingName(action_name, input_name, device, negative_key);
        if(last_text!=text)
        {
            GetComponent<CompText>().SetText(text);
            last_text = text;
        }
    }

}