using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ShowActionName : CulverinBehaviour
{
    string text;
    string action_name;
    string input_name;
    string device;
    bool negative_key;

    void Start()
    {
        text = Input.GetInput_ControllerActionName(action_name, input_name, device, negative_key);
        GetComponent<CompText>().SetText(text);

    }
  
}