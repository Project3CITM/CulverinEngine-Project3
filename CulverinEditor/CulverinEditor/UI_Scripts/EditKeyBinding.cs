using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class EditKeyBinding : CulverinBehaviour
{
    string action_name;
    string input_name;
    string device;
    bool negative_key;

    void Update()
    {
        if (Input.GetInput_KeyDown("GUI", "Submit"))
        {
            Input.SetInputActionToChange(action_name, input_name, device, negative_key);
        }
    }
}
  