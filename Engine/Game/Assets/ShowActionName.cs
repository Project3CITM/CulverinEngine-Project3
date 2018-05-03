using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ShowActionName : CulverinBehaviour
{
    public bool negative_key = false;
    public bool keyboard = false;
    public int string_pos = 0;

    string text;
    string action_name = null;
    string input_name = null;
    int device = 0;

    void Start()
    {
        if (keyboard)
            device = 0;
        else
            device = 1;


        if (string_pos==0)
            action_name = "Vertical";        
        else if (string_pos == 1)        
            action_name = "Horizontal";        
        else if (string_pos == 2)        
            action_name = "Rotate";       
        else if (string_pos == 3)        
            action_name = "TriangleR";        
        else if (string_pos == 4)        
            action_name = "TriangleL";        
        else if (string_pos == 5)        
            action_name = "RAllyAttack";        
        else if (string_pos == 6)        
            action_name = "LAllyAttack";        
        else if (string_pos == 7)        
            action_name = "RAttack";        
        else if (string_pos == 8)       
            action_name = "LAttack";        
        else if (string_pos == 9)        
            action_name = "Interact";        
        else if (string_pos == 10)        
            action_name = "Menu";

        input_name = "Player";


        text = Input.GetInput_ControllerActionName(action_name, input_name, device, negative_key);
        GetComponent<CompText>().SetText(text);
    }
  
}